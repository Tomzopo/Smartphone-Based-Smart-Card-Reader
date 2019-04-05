package sbscr.epr402.paynow;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.ListView;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;

public class PaymentHistory extends AppCompatActivity {
    private static CustomAdapter adapter;
    private static String filename = "SBSCR_history.txt";
    ArrayList<Payment> paymentModels;
    ListView listView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_payment_history);

        listView = (ListView) findViewById(R.id.list);

        paymentModels = new ArrayList<>();
        //

        // For Writing
//        OutputStreamWriter outputWriter;
        File file = new File(getApplicationContext().getFilesDir(), filename);
//        try {
//            FileOutputStream outputStream = new FileOutputStream(file, true);
//            outputWriter = new OutputStreamWriter(outputStream);
//            outputWriter.close();
//        } catch (Exception e) {
//            e.printStackTrace();
//        }

        try {
            FileInputStream inputStream = new FileInputStream(file);
            BufferedReader inputReader = new BufferedReader(new InputStreamReader(inputStream));

            String line;
            line = inputReader.readLine();

            while (line != null) {
                String[] ar = line.split("-");
                paymentModels.add(new Payment(ar[0], ar[1], ar[2], ar[3], ar[4]));
                line = inputReader.readLine();
            }
            if (line == null) {
                inputReader.close();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        adapter = new CustomAdapter(paymentModels, getApplicationContext());
        listView.setAdapter(adapter);
    }

    public void deleteHistory(View view) {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Delete history, are you sure?")
                .setMessage("Your transaction history cannot be recovered.")
                .setIcon(android.R.drawable.ic_dialog_alert)
                .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int whichButton) {
                        deleteFile(filename);
                        adapter.clear();
                        adapter.notifyDataSetChanged();
                        dialog.dismiss();
                    }
                })
                .setNegativeButton("No", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int whichButton) {
                        dialog.cancel();
                    }
                })
                .show();
    }
}

