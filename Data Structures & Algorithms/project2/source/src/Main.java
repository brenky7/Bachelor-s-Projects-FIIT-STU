import string.*;
import testStuff.*;
import tree.*;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Scanner;

public class Main {
    public static HashMap<String, node> nodeMap = new HashMap<String, node>();
    public static void main(String[] args) {
        //String DNF = DNFgenerator.generateDNF(10);
       // bdd tree = BDDcreate(DNF, getVariables.get(DNF));
        //printTree(tree.root, 0);
        //System.out.println("\n\n\n\n\n\n");
        //bdd betterTree = BDDcreate_with_best_order(DNF);
        //printTree(betterTree.root, 0);
        //testTime(18);
        testSpravnosti(10);
    }

    public static bdd BDDcreate(String function, String order){
        nodeMap.put("1", new node("1", '1'));
        nodeMap.put("0", new node("0", '0'));
        bdd tree = new bdd(order);
        function = expressionModifier.check(function);
        tree.root = BDDaddNodes(function, order, 0);
        tree.numberOfVariables = order.length();
        if ( tree.root.left != null && tree.root.right != null && tree.root.left.function.equals(tree.root.right.function)){
            tree.root = tree.root.left;
            tree.root.left = null;
            tree.root.right = null;
        }
        if (tree.root.function == "0"){
            nodeMap.clear();
            nodeMap.put("0", tree.root);
            tree.numberOfNodes = nodeMap.size();
        }
        else if (tree.root.function == "1"){
            nodeMap.clear();
            nodeMap.put("1", tree.root);
            tree.numberOfNodes = nodeMap.size();
        }
        else{
            tree.numberOfNodes = nodeMap.size();
        }
        return tree;
    }

    public static node BDDaddNodes(String function, String order, int index){
        if (function.equals("1")){
            return nodeMap.get("1");
        }
        if (index == order.length()){
            if (function.equals("1")){
                return nodeMap.get("1");
            }
            else if (function.equals("0")){
                return nodeMap.get("0");
            }
            else {
                return null;
            }
        }
        else {
            if (nodeMap.containsKey(function)){
                return nodeMap.get(function);
            }
            else {
                nodeMap.put(function, new node(function, order.charAt(index)));
                String[]next = {" ", " "};
                while (next[0].equals(next[1])){
                    next = SplitFunction.split(function, order.charAt(index));
                    if (!next[0].equals(next[1])){
                        break;
                    }
                    if (next[0].equals("1")){
                        nodeMap.remove(function);
                        return nodeMap.get("1");
                    }
                    else if (next[0].equals("0")){
                        nodeMap.remove(function);
                        return nodeMap.get("0");
                    }
                    else  {
                        index++;
                        nodeMap.get(function).variable = order.charAt(index);
                    }

                }
                nodeMap.get(function).left = BDDaddNodes(next[0], order, index + 1);
                nodeMap.get(function).right = BDDaddNodes(next[1], order, index + 1);
                if (nodeMap.get(function).left == nodeMap.get(function).right){
                    node temp = nodeMap.get(function);
                    nodeMap.get(function).function = nodeMap.get(function).left.function;
                    nodeMap.get(function).left = null;
                    nodeMap.get(function).right = null;
                    nodeMap.remove(temp);
                }
                return nodeMap.get(function);
            }
        }
    }
    public static bdd BDDcreate_with_best_order(String function){
        bdd tree = new bdd();
        bdd demoTree;
        String variables = getVariables.get(function);
        String[] orders = orderShuffle.shuffle(variables);
        double minNodes = Double.POSITIVE_INFINITY;
        for (String i : orders){
            demoTree = BDDcreate(function, i);
            if (demoTree.numberOfNodes < minNodes){
                minNodes = demoTree.numberOfNodes;
                tree = demoTree;
            }
            nodeMap.clear();
        }
        return tree;
    }
    public static void printTree(node root, int level) {
        if (root != null) {
            printTree(root.right, level + 1);
            System.out.println(" ".repeat(4 * level) + "-> " +"(" + root.variable + ")" + root.function);
            printTree(root.left, level + 1);
        }
    }
    public static int BDD_use(bdd tree, String input){
        node current = tree.root;
        int index = 0;
        while (current != null){
            if (input.charAt(index) == '0'){
                if (current.variable == tree.order.toCharArray()[index]) {
                    current = current.left;
                }
            }
            else if (input.charAt(index) == '1') {
                if (current.variable == tree.order.toCharArray()[index]) {
                    current = current.right;
                }
            }
            else {
                System.out.println("Invalid input");
                return -1;
            }
            if (current.function.equals("0") || current.function.equals("1")){
                return Integer.parseInt(current.function);
            }
            index++;
        }
        return Integer.parseInt(current.function);
    }
    public static void testSpravnosti(int numberOfVariables){
        try{
            Scanner scanner = new Scanner(new File("DNF"+numberOfVariables+".txt"));
            List<String> list = DNFgenerator.generate(numberOfVariables);
            while (scanner.hasNextLine()){
                bdd tree;
                String DNF = scanner.nextLine();
                tree = BDDcreate(DNF, getVariables.get(DNF));
                for (String j : list){
                    if (BDD_use(tree, j) != expressionEvaluator.evaluate(DNF, j)){
                        System.out.println("Error");
                    }
                }
                nodeMap.clear();
            }
        }
        catch (Exception e){
            System.out.println("File error");
        }
    }

    public static void testTime(int numberOfVariables){
        String[] testArr = new String[100];
        double[] createArr = new double[100];
        double[] createWBOArr = new double[100];
        double[] createPercentArr = new double[100];
        double[] createWBOPercentArr = new double[100];
        long startTime = System.nanoTime();
        try{
            Scanner scanner = new Scanner(new File("DNF"+numberOfVariables+".txt"));
            int index = 0;
            while (scanner.hasNextLine()){
                bdd tree;
                bdd betterTree;
                String DNF = scanner.nextLine();
                tree = BDDcreate(DNF, getVariables.get(DNF));
                nodeMap.clear();
                betterTree = BDDcreate_with_best_order(DNF);
                nodeMap.clear();
                double maxNodes = Math.pow(2, numberOfVariables+1)-1;
                double create = tree.numberOfNodes;
                double createWBO = betterTree.numberOfNodes;
                testArr[index] = String.format("Max nodes: %.0f, create: %.0f, create with best order: %.0f\nPercento redukcie create: %.2f%%, percento redukcie create with best order: %.2f%%",
                        maxNodes, create, createWBO,
                        ((maxNodes - create) / maxNodes) * 100, ((create - createWBO) / create) * 100);
                createArr[index] = create;
                createWBOArr[index] = createWBO;
                createPercentArr[index] = (maxNodes - create) / maxNodes * 100;
                createWBOPercentArr[index] = (create - createWBO) / create * 100;
                index++;
            }
            long endTime = System.nanoTime();
            for (String i : testArr){
                System.out.println(i);
            }
            System.out.println("Total time: " + (endTime - startTime) / 1000000 + "ms");
            System.out.println("Average time: " + (endTime - startTime) / 1000000 / 100 + "ms");
            System.out.println("Average percento redukcie create: " + Arrays.stream(createPercentArr).average().getAsDouble() + "%");
            System.out.println("Average percento redukcie create with best order: " + Arrays.stream(createWBOPercentArr).average().getAsDouble() + "%");
            System.out.println("Average create nodes: " + Arrays.stream(createArr).average().getAsDouble());
            System.out.println("Average create with best order nodes: " + Arrays.stream(createWBOArr).average().getAsDouble());
        }
        catch (FileNotFoundException e){
            System.out.println("File error");
        }
    }

}