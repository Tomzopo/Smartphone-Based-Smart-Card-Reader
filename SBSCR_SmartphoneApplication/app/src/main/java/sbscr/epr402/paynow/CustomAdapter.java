package sbscr.epr402.paynow;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import java.util.ArrayList;

public class CustomAdapter extends ArrayAdapter<Payment>{
    Context mContext;
    private ArrayList<Payment> paymentSet;
    private int lastPosition = -1;

    public CustomAdapter(ArrayList<Payment> payment, Context context) {
        super(context, R.layout.row_layout, payment);
        this.paymentSet = payment;
        this.mContext = context;

    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        // Get the data item for this position
        Payment paymentModel = getItem(position);
        // Check if an existing view is being reused, otherwise inflate the view
        ViewHolder viewHolder; // view lookup cache stored in tag

        final View result;

        if (convertView == null) {

            viewHolder = new ViewHolder();
            LayoutInflater inflater = LayoutInflater.from(getContext());
            convertView = inflater.inflate(R.layout.row_layout, parent, false);
            viewHolder.txtID = (TextView) convertView.findViewById(R.id.ID);
            viewHolder.txtProduct = (TextView) convertView.findViewById(R.id.product);
            viewHolder.txtCost = (TextView) convertView.findViewById(R.id.cost);
            viewHolder.txtCalendarDate = (TextView) convertView.findViewById(R.id.calendarDate);
            viewHolder.txtCalendarTime = (TextView) convertView.findViewById(R.id.calendarTime);
            result = convertView;

            convertView.setTag(viewHolder);
        } else {
            viewHolder = (ViewHolder) convertView.getTag();
            result = convertView;
        }

        lastPosition = position;

        viewHolder.txtID.setText("#" + paymentModel.getID());
        viewHolder.txtProduct.setText(paymentModel.getProduct());
        viewHolder.txtCost.setText("R" + paymentModel.getCost());
        viewHolder.txtCalendarDate.setText(paymentModel.getCalendarDate());
        viewHolder.txtCalendarTime.setText(paymentModel.getCalendarTime());
        // Return the completed view to render on screen
        return convertView;
    }

    private static class ViewHolder {
        TextView txtID;
        TextView txtProduct;
        TextView txtCost;
        TextView txtCalendarDate;
        TextView txtCalendarTime;
    }
}
