package sbscr.epr402.paynow;


import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.support.design.widget.CoordinatorLayout;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

public class PaymentPage extends AppCompatActivity {
    // Extras
    public static final String transaction_Name_EXTRA = "SBSCR.EPR402.Paynow.TRANSACTION_NAME";
    public static final double transaction_Cost_EXTRA = 0.00;

    // Widgets
    CoordinatorLayout coordinatorLayout = null;
    Snackbar mSnackbar = null;
    EditText edit_Cost = null;
    EditText edit_Name = null;

    // Preferences
    SharedPreferences prefs = null;
    String deviceHardwareAddress = "";
    String deviceName = "";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_payment_page);

        //Widgets
        coordinatorLayout = (CoordinatorLayout) findViewById(R.id.coordinatorLayout);
        edit_Name = (EditText) findViewById(R.id.text_Name);
        edit_Name.requestFocus();

        //Preferences
        prefs = getSharedPreferences("Bluetooth_Details", MODE_PRIVATE);

        deviceHardwareAddress = prefs.getString("Bluetooth_Address", "NO BLUETOOTH DEVICE SELECTED");
        deviceName = prefs.getString("Bluetooth_Name", "NO BLUETOOTH DEVICE SELECTED");

        mSnackbar = Snackbar.make(coordinatorLayout, deviceName + " " + deviceHardwareAddress, Snackbar.LENGTH_INDEFINITE);
        //mSnackbar.show();
    }

    public void sendPayment(View view) {
        //Add checking for viable amount

        edit_Cost = (EditText) findViewById(R.id.text_Cost);
        edit_Name = (EditText) findViewById(R.id.text_Name);

        //Would like to add a transaction ID
        String cost_Str = edit_Cost.getText().toString();
        String name = edit_Name.getText().toString();

        //Check Name and Cost;
        double cost;
        try {
            cost = Double.parseDouble(cost_Str);
        } catch (NumberFormatException e) {
            cost = 1;
        }
        if (name.equals(""))
            name = "Default";
        if (cost > 10000.00) {
            Toast.makeText(this, "Cost must be less than R10000.00", Toast.LENGTH_LONG).show();
        } else {
            Intent intent = new Intent(this, TransactionPage.class);

            intent.putExtra(transaction_Name_EXTRA, name);
            intent.putExtra("transaction_Cost_EXTRA", cost);

            startActivity(intent);
        }


    }

}
