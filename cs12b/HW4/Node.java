// Node.java
// Author: Robert Bergeron
// ID: rbergero
// Node class for holding items in the queue

public class Node {

    private Object item;
    private Node nextNode = null;

    public Node(Object obj) {
        this.item = obj;
    }

    public Object getItem() {
        return item;
    }

    public Node getNext() {
        return nextNode;
    }

    public void addNext(Object obj) {
        if(this.nextNode == null) {
            this.nextNode = new Node(obj);
        }
        else {
            this.nextNode.addNext(obj);
        }
    }
}
