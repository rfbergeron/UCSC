//  Entry
//  Author: Robert Bergeron
//  ID: rbergero
//  Represents individual entries within a Stack
//----------------------------------------------------------------------------------------------

public class Entry {
    private char item;
    private Entry next;
    private int position;

    public Entry(char c, int pos) {
        this.item = c;
        this.position = pos;
    }

    public Entry(char c, int pos, Entry nextEntry) {
        this.item = c;
        this.position = pos;
        this.setNext(nextEntry);
    }

    public void setNext(Entry entry) {
        this.next = entry;
    }

    public Entry getNext() {
        return this.next;
    }

    public int getPosition() {
        return this.position;
    }

    public char getItem() {
        return item;
    }

}
