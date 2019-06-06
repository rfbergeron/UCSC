// QueueEvent
// Author: Robert Bergeron
// ID: rbergero
// Objects representing data to be stored in queued nodes, usually begin
// or end markers

public class QueueEvent {

    String name;
    String action;
    int time;

    public QueueEvent(String n, String a, int t) {
        this.time = t;
        this.name = n;
        this.action = a;
    }

    public String toString() {
        return name + action + time;
    }
}
