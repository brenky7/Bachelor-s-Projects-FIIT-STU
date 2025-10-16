package tree;

public class bdd {
    public int numberOfNodes;
    public node root;
    public String order;
    public int numberOfVariables;
    public bdd(String order){
        this.numberOfNodes = 0;
        this.root = null;
        this.order = order;
    }
    public bdd(){
        this.numberOfNodes = 0;
        this.root = null;
    }

    public void destruct(){
        this.root = null;
        this.numberOfNodes = 0;
        this.order = null;
    }
}
