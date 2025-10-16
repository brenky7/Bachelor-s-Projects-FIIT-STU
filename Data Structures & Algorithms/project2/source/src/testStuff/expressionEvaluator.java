package testStuff;

import string.getVariables;

public class expressionEvaluator {
    public static int evaluate(String dnf, String number){
        char[] orderArr = getVariables.get(dnf).toCharArray();
        char[] numberArr = number.toCharArray();
        for (int i = 0; i < number.length(); i++){
            if (dnf.contains("!" + orderArr[i])){
                if (numberArr[i] == '0'){
                    dnf = dnf.replace("!" + Character.toString(orderArr[i]), "1");
                }
                else {
                    dnf = dnf.replace("!" + Character.toString(orderArr[i]), "0");
                }
            }
            if (dnf.contains(Character.toString(orderArr[i]))){
                if (numberArr[i] == '1'){
                    dnf = dnf.replace(Character.toString(orderArr[i]), "1");
                }
                else {
                    dnf = dnf.replace(Character.toString(orderArr[i]), "0");
                }
            }
        }
        String[] dnfSplit = dnf.split("\\+");
        boolean allOnes = false;
        for (String i: dnfSplit){
            if (!i.contains("0")){
                allOnes = true;
                break;
            }
        }
        if (allOnes){
            return 1;
        }
        else{
            return 0;
        }
    }
}
