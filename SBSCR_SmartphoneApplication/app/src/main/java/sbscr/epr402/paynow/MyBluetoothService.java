package sbscr.epr402.paynow;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;

public class MyBluetoothService {
    // Message Constants
    public static final int MessageConstants_STATE = 0;
    public static final int MessageConstants_READ = 1;
    public static final int MessageConstants_WRITE = 2;
    public static final int MessageConstants_TOAST = 3;
    public static final int MessageConstants_CANCEL = 4;
    public static final int MessageConstants_START = 5;
    // Bluetooth States
    public static final int StateConstants_NONE = 0;
    public static final int StateConstants_CONNECTING = 1;
    public static final int StateConstants_CONNECTED = 2;
    // Bluetooth
    private static final UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    // Debugging
    private static final String TAG = "BTService_Act";
    // Activity Variables
    private int mState;
    private BluetoothAdapter mBluetoothAdapter = null;
    private ConnectThread mConnectThread = null;
    private ConnectedThread mConnectedThread = null;
    // Handler
    private Handler mHandler;

    public MyBluetoothService(Context context, Handler handler) {
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        mHandler = handler;
        mState = StateConstants_NONE;
    }

    public synchronized int getState() {
        return mState;
    }

    public synchronized void setState(int state) {
        mState = state;
        mHandler.obtainMessage(TransactionPage.MessageConstants_STATE, state - 1).sendToTarget();
    }

    public synchronized void connectThread(BluetoothDevice bt_device) {
        if (mState == StateConstants_CONNECTING) {
            if (mConnectThread != null) {
                mConnectThread.cancel();
                mConnectThread = null;
            }
        }

        if (mConnectedThread != null) {
            mConnectedThread.cancel();
            mConnectedThread = null;
        }

        mConnectThread = new ConnectThread(bt_device);
        mConnectThread.start();
        setState(StateConstants_CONNECTING);
    }

    public synchronized void connectedThread(BluetoothSocket mmSocket) {
        cancel();

        mConnectedThread = new ConnectedThread(mmSocket);
        mConnectedThread.start();

        Message connected_msg = mHandler.obtainMessage(TransactionPage.MessageConstants_TOAST);
        Bundle bundle = new Bundle();
        bundle.putString(TransactionPage.TOAST_MSG, "Payment Request Sent");
        connected_msg.setData(bundle);
        mHandler.sendMessage(connected_msg);
        setState(StateConstants_CONNECTED);
    }

    public synchronized void cancel() {
        if (mConnectThread != null) {
            mConnectThread.cancel();
            mConnectThread = null;
        }
        if (mConnectedThread != null) {
            mConnectedThread.cancel();
            mConnectedThread = null;
        }
        setState(StateConstants_NONE);
    }

    public void write(byte[] msg) {
        ConnectedThread temp;
        synchronized (this) {
            if (mState != StateConstants_CONNECTED)
                return;
            temp = mConnectedThread;
        }
        temp.write(msg);
    }

    private void connectingFailed() {
        setState(StateConstants_NONE);
        Message disconnect_msg = mHandler.obtainMessage(TransactionPage.MessageConstants_CANCEL);
        Bundle bundle = new Bundle();
        bundle.putString(TransactionPage.TOAST_MSG, "Payment request failed");
        cancel();
        disconnect_msg.setData(bundle);
        mHandler.sendMessage(disconnect_msg);
    }

    private void connectionLost() {
        setState(StateConstants_NONE);
        Message disconnect_msg = mHandler.obtainMessage(TransactionPage.MessageConstants_TOAST);
        Bundle bundle = new Bundle();
        bundle.putString(TransactionPage.TOAST_MSG, "Bluetooth connection has been lost");
        disconnect_msg.setData(bundle);
        //mHandler.sendMessage(disconnect_msg);
    }

    private class ConnectThread extends Thread {
        private final BluetoothSocket mmSocket;
        private final BluetoothDevice mmDevice;

        public ConnectThread(BluetoothDevice device) {
            // Use a temporary object that is later assigned to mmSocket
            // because mmSocket is final.
            BluetoothSocket tmp = null;
            mmDevice = device;

            try {
                // Get a BluetoothSocket to connect with the given BluetoothDevice.
                // MY_UUID is the app's UUID string, also used in the server code.
                tmp = device.createRfcommSocketToServiceRecord(MY_UUID);
            } catch (IOException e) {
                Log.e(TAG, "Socket's create() method failed", e);
            }
            mmSocket = tmp;
        }

        public void run() {
            mBluetoothAdapter.cancelDiscovery();

            try {
                // Connect to the remote device through the socket. This call blocks
                // until it succeeds or throws an exception.
                mmSocket.connect();
            } catch (IOException connectException) {
                connectingFailed();
                try {
                    mmSocket.close();
                } catch (IOException closeException) {
                    Log.e(TAG, "Could not close the client socket", closeException);
                }
                return;
            }

            synchronized (MyBluetoothService.this) {
                mConnectThread = null;
            }
            // The connection attempt succeeded. Perform work associated with
            // the connection in a separate thread.
            connectedThread(mmSocket);
        }

        // Closes the client socket and causes the thread to finish.
        public void cancel() {
            try {
                mmSocket.close();
            } catch (IOException e) {
                Log.e(TAG, "Could not close the client socket", e);
            }
        }
    }

    private class ConnectedThread extends Thread {
        private final BluetoothSocket mmSocket;
        private final InputStream mmInStream;
        private final OutputStream mmOutStream;

        public ConnectedThread(BluetoothSocket socket) {
            mmSocket = socket;
            InputStream tmpIn = null;
            OutputStream tmpOut = null;

            // Get the input and output streams; using temp objects because
            // member streams are final.
            try {
                tmpIn = socket.getInputStream();
            } catch (IOException e) {
                Log.e(TAG, "Error occurred when creating input stream", e);
            }
            try {
                tmpOut = socket.getOutputStream();
            } catch (IOException e) {
                Log.e(TAG, "Error occurred when creating output stream", e);
            }

            mmInStream = tmpIn;
            mmOutStream = tmpOut;

            Message start_msg = mHandler.obtainMessage(TransactionPage.MessageConstants_START);
            Bundle bundle = new Bundle();
            start_msg.setData(bundle);
            mHandler.sendMessage(start_msg);
        }

        public void run() {
            byte[] mmBuffer = new byte[1024];
            int numBytes = 0; // bytes returned from read()
            int begin = 0;
            // Keep listening to the InputStream until an exception occurs.
            while (true) {
                try {
                    // Read from the InputStream.
                    numBytes += mmInStream.read(mmBuffer, numBytes, mmBuffer.length - numBytes);
                    for (int i = begin; i < numBytes; i++) {
                        if (mmBuffer[i] == "\r".getBytes()[0]) {
                            mHandler.obtainMessage(MessageConstants_READ, begin, i, mmBuffer).sendToTarget();
                            begin = i + 1;
                            if (i == numBytes - 1) {
                                numBytes = 0;
                                begin = 0;
                            }
                        }
                    }
                } catch (IOException e) {
                    connectionLost();
                    break;
                }
            }
        }

        // Call this from the main activity to send data to the remote device.
        public void write(byte[] bytes) {
            try {
                mmOutStream.write(bytes);

                // Share the sent message with the UI activity.
                Message writtenMsg = mHandler.obtainMessage(
                        MessageConstants_WRITE, -1, -1, bytes);
                writtenMsg.sendToTarget();
            } catch (IOException e) {
                Log.e(TAG, "Error occurred when sending data", e);

                // Send a failure message back to the activity.
                Message writeErrorMsg =
                        mHandler.obtainMessage(MessageConstants_TOAST);
                Bundle bundle = new Bundle();
                bundle.putString("TOAST_MSG",
                        "Couldn't send data to the other device");
                writeErrorMsg.setData(bundle);
                mHandler.sendMessage(writeErrorMsg);
            }
        }

        // Call this method from the main activity to shut down the connection.
        public void cancel() {
            try {
                mmSocket.close();
            } catch (IOException e) {
                Log.e(TAG, "Could not close the connect socket", e);
            }
        }
    }
}
