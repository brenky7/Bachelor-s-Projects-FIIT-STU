package tree;

public class node {
    public String function;
    public char variable;
    public node left;
    public node right;
    public node(String function, char variable){
        this.function = function;
        this.variable = variable;
        this.left = null;
        this.right = null;
    }
}
