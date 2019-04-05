package sbscr.epr402.paynow;

import android.app.Activity;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.support.design.widget.CoordinatorLayout;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import java.util.Set;

public class MainActivity extends AppCompatActivity {
    // Bluetooth
    private final static int REQUEST_ENABLE_BT = 1;
    String deviceHardwareAddress = "";
    String deviceName = "";
    int deviceNum = 0;

    // Widgets
    Button btn_RequestPayments = null;
    Button btn_PaymentHistory = null;
    Button btn_Settings = null;
    CoordinatorLayout coordinatorLayout = null;
    Snackbar mSnackbar = null;

    // Preferences
    SharedPreferences prefs = null;
    private BluetoothAdapter mBluetoothAdapter = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Preferences
        prefs = getSharedPreferences("Bluetooth_Details", MODE_PRIVATE);

        deviceHardwareAddress = prefs.getString("Bluetooth_Address", "NO BLUETOOTH DEVICE SELECTED");
        deviceName = prefs.getString("Bluetooth_Name", "");

        //Widgets
        btn_RequestPayments = (Button) findViewById(R.id.button_RequestPayment);
        btn_PaymentHistory = (Button) findViewById(R.id.button_History);
        btn_Settings = (Button) findViewById(R.id.button_Settings);
        coordinatorLayout = (CoordinatorLayout) findViewById(R.id.coordinatorLayout);

        //Bluetooth
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        if (mBluetoothAdapter == null) {
            Toast.makeText(this, "Bluetooth is required", Toast.LENGTH_LONG).show();
            finish();
        } else if (!mBluetoothAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        }

        mSnackbar = Snackbar.make(coordinatorLayout, deviceName + " " + deviceHardwareAddress, Snackbar.LENGTH_INDEFINITE);
    }

    public void requestPayments_Activity(View view) {
        Intent intent = new Intent(this, PaymentPage.class);
        startActivity(intent);
    }

    public void settings_Activity(View view) {

        //Device List
        Set<BluetoothDevice> pairedDevices = mBluetoothAdapter.getBondedDevices();

        final String[] btDevicesName = new String[pairedDevices.size()];
        final String[] btDevicesAddress = new String[pairedDevices.size()];
        int deviceCount = 0;
        // List of currently paired devices
        if (pairedDevices.size() > 0) {
            // There are paired devices. Get the name and address of each paired device.
            for (BluetoothDevice device : pairedDevices) {
                btDevicesName[deviceCount] = device.getName(); // Name
                btDevicesAddress[deviceCount] = device.getAddress(); // MAC address
                deviceCount++;
            }
        }

        AlertDialog.Builder builder = new AlertDialog.Builder(this);

        for (int i = 0; i < pairedDevices.size(); i++) {
            if (btDevicesAddress[i].equals(deviceHardwareAddress))
                deviceNum = i;
        }

        if (pairedDevices.size() > 0) {
            builder.setTitle("Select Bluetooth Device")
                    .setSingleChoiceItems(btDevicesName, deviceNum, null)
                    .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int whichButton) {
                            int selectedPosition = ((AlertDialog) dialog).getListView().getCheckedItemPosition();
                            deviceHardwareAddress = btDevicesAddress[selectedPosition];
                            deviceName = btDevicesName[selectedPosition];
                            SharedPreferences.Editor editor = prefs.edit();
                            editor.putString("Bluetooth_Address", deviceHardwareAddress);
                            editor.putString("Bluetooth_Name", deviceName);
                            mSnackbar.setText(deviceName + " " + deviceHardwareAddress);
                            //mSnackbar.show();
                            editor.commit();
                            dialog.dismiss();
                        }
                    })
                    .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int whichButton) {
                            dialog.cancel();
                        }
                    })
                    .show();
        } else {
            Toast.makeText(getApplicationContext(), "No Paired Bluetooth Devices Found.", Toast.LENGTH_LONG).show();
        }
    }

    public void paymentHistory_Activity(View view) {
        Intent intent = new Intent(this, PaymentHistory.class);
        startActivity(intent);
    }

    //Checks states of activities
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        switch (requestCode) {
            case REQUEST_ENABLE_BT:
                // When the request to enable Bluetooth returns
                if (resultCode == Activity.RESULT_OK) {
                    // Bluetooth was turned on
                    Toast.makeText(this, "Bluetooth is now available", Toast.LENGTH_LONG).show();
                    mSnackbar.show();
                } else {
                    // Bluetooth was not turned on
                    Toast.makeText(this, "Bluetooth is required", Toast.LENGTH_LONG).show();
                    finish();
                }
        }
    }
}


