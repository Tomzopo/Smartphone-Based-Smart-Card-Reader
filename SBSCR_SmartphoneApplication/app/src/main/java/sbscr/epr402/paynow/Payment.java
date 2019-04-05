package sbscr.epr402.paynow;

public class Payment {
    String id;
    String product;
    String cost;
    String calendarDate;
    String calendarTime;

    Payment(String id, String product, String cost, String calendarDate, String calendarTime) {
        this.id = id;
        this.product = product;
        this.cost = cost;
        this.calendarDate = calendarDate;
        this.calendarTime = calendarTime;
    }

    public String getID() {
        return this.id;
    }

    public String getProduct() {
        return this.product;
    }

    public String getCost() {
        return this.cost;
    }

    public String getCalendarDate() {
        return this.calendarDate;
    }

    public String getCalendarTime() {
        return this.calendarTime;
    }
}