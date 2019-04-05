package sbscr.epr402.paynow;

import android.app.Activity;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.design.widget.CoordinatorLayout;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.text.DecimalFormat;
import java.util.Calendar;

public class TransactionPage extends AppCompatActivity {
    // Activity Variables
    public static final String TOAST_MSG = "";
    // Message Constants
    public static final int MessageConstants_STATE = 0;
    public static final int MessageConstants_READ = 1;
    public static final int MessageConstants_WRITE = 2;
    public static final int MessageConstants_TOAST = 3;
    public static final int MessageConstants_CANCEL = 4;
    public static final int MessageConstants_START = 5;
    // Terminal Messages
    public static final String TerminalMessages_CONNECTED = "SBSCR_CONNECTED";
    public static final String TerminalMessages_PAYMENTREQUEST = "SBSCR_PAYMENTREQUEST";
    public static final String TerminalMessages_PAYMENTACCEPTED = "SBSCR_ACCEPTED";
    public static final String TerminalMessages_CANCEL = "SBSCR_CANCEL";
    public static final String TerminalMessages_FAILED = "SBSCR_FAILED";
    public static final String TerminalMessages_TEST = "TESTING_MESSAGE";
    // Bluetooth
    private static final int REQUEST_ENABLE_BT = 1;
    // File
    private static String filename = "SBSCR_history.txt";
    // EXTRAS
    String transaction_Name = "Default";
    double transaction_Cost = 0.00;
    // Widgets
    TextView text_Details = null;
    Button btn_Cancel = null;
    Button btn_Test = null;
    CoordinatorLayout coordinatorLayout = null;
    Snackbar mSnackbar = null;
    ProgressBar mProgressBar1 = null;
    ProgressBar mProgressBar2 = null;
    // Handler
    // Preferences
    SharedPreferences prefs = null;
    String deviceHardwareAddress = "";
    String deviceName = "";
    private BluetoothAdapter mBluetoothAdapter = null;
    private MyBluetoothService mBluetoothService = null;
    private final Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message message) {
            switch (message.what) {
                case MessageConstants_STATE:
                    break;

                case MessageConstants_START:
                    paymentRequest();
                    break;

                case MessageConstants_READ:
                    byte[] readBuf = (byte[]) message.obj;
                    int begin = message.arg1;
                    int end = message.arg2;
                    String readMessage = new String(readBuf).substring(begin, end);
                    receivedCommands(readMessage);
                    break;

                // For debugging purposes
                case MessageConstants_WRITE:
                    byte[] writeBuf = (byte[]) message.obj;
                    String writeMessage = new String(writeBuf);
                    break;

                case MessageConstants_TOAST:
                    Toast.makeText(getApplicationContext(), message.getData().getString(TOAST_MSG), Toast.LENGTH_SHORT).show();
                    break;

                case MessageConstants_CANCEL:
                    Toast.makeText(getApplicationContext(), message.getData().getString(TOAST_MSG), Toast.LENGTH_SHORT).show();
                    finish();
                    break;
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_transaction_page);

        // Preferences
        prefs = getSharedPreferences("Bluetooth_Details", MODE_PRIVATE);
        deviceHardwareAddress = prefs.getString("Bluetooth_Address", "NO BLUETOOTH DEVICE SELECTED");
        deviceName = prefs.getString("Bluetooth_Name", "NO BLUETOOTH DEVICE SELECTED");

        Intent intent = getIntent();
        transaction_Name = intent.getStringExtra(PaymentPage.transaction_Name_EXTRA);
        transaction_Cost = intent.getDoubleExtra("transaction_Cost_EXTRA", PaymentPage.transaction_Cost_EXTRA);

        // Bluetooth
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        // Widgets
        coordinatorLayout = (CoordinatorLayout) findViewById(R.id.coordinatorLayout);
        btn_Cancel = (Button) findViewById(R.id.btn_Cancel);
        text_Details = (TextView) findViewById(R.id.text_TransactionDetails);
        text_Details.setText("Name: " + transaction_Name + "\nCost: R" + Double.toString(transaction_Cost));

        mSnackbar = Snackbar.make(coordinatorLayout, deviceName + " " + deviceHardwareAddress, Snackbar.LENGTH_INDEFINITE);
        mProgressBar1 = (ProgressBar) findViewById(R.id.progressbar_transaction);
        mProgressBar2 = (ProgressBar) findViewById(R.id.progressbar_transaction2);
        mProgressBar2.setVisibility(View.INVISIBLE);
        mProgressBar1.setScaleY(3f);
        mProgressBar1.setMax(100);
    }

    @Override
    public void onStart() {
        super.onStart();
        if (!mBluetoothAdapter.isEnabled()) {
            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
        } else {
            if (mBluetoothService == null) setupTransaction();
        }
    }

    @Override
    public synchronized void onPause() {
        super.onPause();
    }

    @Override
    public synchronized void onDestroy() {
        super.onDestroy();
        if (mBluetoothService != null) ;
        //mBluetoothService.stop();
    }

    public void setupTransaction() {
        mBluetoothService = new MyBluetoothService(this, mHandler);
        BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(deviceHardwareAddress);
        mBluetoothService.connectThread(device);

        //while (mBluetoothService.getState() == mBluetoothService.StateConstants_CONNECTING) ;

        bluetoothWrite(TerminalMessages_CONNECTED);
    }

    public void bluetoothWrite(String str) {
//        if (mBluetoothService.getState() != mBluetoothService.StateConstants_CONNECTED) {
//            Toast.makeText(this, "Bluetooth not connected", Toast.LENGTH_SHORT).show();
//            return;
//        }
        if (str.length() > 0) {
            byte[] msg = (str + "\0\r").getBytes();
            mBluetoothService.write(msg);
        }

    }

    public void CancelPayment(View view) {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Cancel Transaction.\nAre you sure?")
                .setMessage("Item: " + transaction_Name + "\nCost: R" + transaction_Cost)
                .setIcon(android.R.drawable.ic_dialog_alert)
                .setCancelable(false)
                .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int whichButton) {
                        bluetoothWrite(TerminalMessages_CANCEL);
                        //mBluetoothService.cancel();
                        Toast.makeText(TransactionPage.this, "Transaction Cancelled", Toast.LENGTH_SHORT).show();
                        finish();
                    }
                })
                .setNegativeButton("No", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int whichButton) {
                        dialog.cancel();
                    }
                })
                .show();
    }

    public void receivedCommands(String cmd) {
        if (cmd.equals("Clear"))
            text_Details.setText("");
        else if (cmd.equals(TerminalMessages_PAYMENTACCEPTED)) {
            addToPaymentHistory(transaction_Name, transaction_Cost);
            mProgressBar1.setProgress(100);
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setMessage("The payment has been made.")
                    .setCancelable(false)
                    .setTitle("Transaction Succesful.")
                    .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int whichButton) {
                            finish();
                        }
                    });
            builder.show();

        } else if (cmd.equals(TerminalMessages_CANCEL)) {
            Toast.makeText(this, "The payment was CANCELLED", Toast.LENGTH_SHORT).show();
            finish();
        } else if (cmd.equals(TerminalMessages_FAILED)) {
            Toast.makeText(this, "The payment failed.\n Please try again.", Toast.LENGTH_SHORT).show();
            mBluetoothService.cancel();
            finish();
        } else if (cmd.equals("SBSCR_GETNAME")) {
            setName();
        } else if (cmd.equals("SBSCR_GETCOST")) {
            setCost();
            mProgressBar2.setVisibility(View.VISIBLE);
        } else {
            Toast.makeText(this, "Unknown Command Received\n\"" + cmd + "\"", Toast.LENGTH_SHORT).show();
        }
    }

    public void paymentRequest() {
        bluetoothWrite(TerminalMessages_PAYMENTREQUEST);
    }

    public void setName() {
        bluetoothWrite("SBSCR_NAME: " + transaction_Name);
    }

    public void setCost() {
        bluetoothWrite("SBSCR_COST: " + Double.toString(transaction_Cost));
    }

    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        switch (requestCode) {
            case REQUEST_ENABLE_BT:
                // When the request to enable Bluetooth returns
                if (resultCode != Activity.RESULT_OK) {
                    // Bluetooth was not turned on
                    Toast.makeText(this, "Bluetooth is required", Toast.LENGTH_SHORT).show();
                    finish();
                }
        }
    }

    public void addToPaymentHistory(String name, Double cost) {
        OutputStreamWriter outputWriter;

        File file = new File(getApplicationContext().getFilesDir(), filename);

        String OldID = "0";
        //Get ID number
        try {
            FileInputStream inputStream = new FileInputStream(file);
            BufferedReader inputReader = new BufferedReader(new InputStreamReader(inputStream));

            String line;
            line = inputReader.readLine();

            while (line != null) {
                String[] ar = line.split("-");
                OldID = ar[0];
                line = inputReader.readLine();
            }
            if (line == null) {
                inputReader.close();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        int ID_num;

        if (OldID.equals("0"))
            ID_num = 0;
        else
            ID_num = Integer.parseInt(OldID) + 1;

        String ID = String.format("%05d", ID_num);
        DecimalFormat decFor = new DecimalFormat("0.00");
        String finalCost = decFor.format(cost);

        try {
            FileOutputStream outputStream = new FileOutputStream(file, true);
            outputWriter = new OutputStreamWriter(outputStream);
            Calendar now = Calendar.getInstance();
            outputWriter.append(ID + "-" + name + "-" + finalCost + "-" + String.format("%1$tD-%1$tH:%1$tM:%1$tS", now) + "\n");
            outputWriter.close();
        } catch (Exception e) {
            Toast.makeText(this, "Error writing to history", Toast.LENGTH_LONG).show();
            e.printStackTrace();
        }
    }
}