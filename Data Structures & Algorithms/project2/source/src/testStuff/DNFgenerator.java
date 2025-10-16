package testStuff;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Random;
public class DNFgenerator {
    public static List<String> generate(int numberOfVariables){
        String DNF = "";
        for (int i = 0; i < Math.pow(2, numberOfVariables); i++){
            String binary = Integer.toBinaryString(i);
            while (binary.length() < numberOfVariables){
                binary = "0" + binary;
            }
            DNF += binary + "+";
        }
        String[] arr = DNF.split("\\+");
        ArrayList<String> list = new ArrayList<String>(Arrays.asList(arr));
        return list;
    }

    public static List<String> generateAll(int numberOfVariables){
        List<String> finalList = new ArrayList<String>();
        for(int i = numberOfVariables; i > 0; i-- ){
            List<String> part = generate(i);
            for (String j : part){
                finalList.add(j);
            }
        }
        return finalList;
    }

    public static List<String> choose(int  numberOfVariables){
        List<String> fullList = generateAll(numberOfVariables);
        List<String> myList = new ArrayList<String>();
        boolean one = false;
        Random random = new Random();
        int size = random.nextInt(numberOfVariables+5);
        if (size == 0){
            size = 3;
        }
        for (int i = 0; i < size-1; i++){
            int index = random.nextInt(fullList.size());
            String element = fullList.get(index);
            if (!myList.contains(element)){
                if (element.length() == 1 && !one){
                    myList.add(element);
                    one = true;
                }
                else if (element.length() > 1){
                    myList.add(element);
                }
            }
        }
        myList.add(fullList.get(random.nextInt(numberOfVariables)));
        return myList;
    }

    public static char[] dajPismena(int position) {
        int length = position;
        char[] result = new char[length];
        for (int i = 0; i < length; i++) {
            result[i] = (char)('A' + i);
        }
        return result;
    }


    public static String generateDNF(int numberOfVariables){
        char[] variables = dajPismena(numberOfVariables);
        List <String> binarylist = choose(numberOfVariables);
        List <String> DNFlist = new ArrayList<String>();
        for (String i : binarylist){
            String temp = "";
            if (i.length() == numberOfVariables){
                for (int j = 0; j < i.length(); j++){
                    if (i.charAt(j) == '0'){
                        temp += "!" + variables[j];
                    }
                    else {
                        temp += variables[j];
                    }
                }
                DNFlist.add(temp);
            }
            else {
                for (int j = 0; j < i.length(); j++){
                    if (i.charAt(j) == '0'){
                        temp += "!" + variables[j+1];
                    }
                    else {
                        temp += variables[j+1];
                    }
                }
                DNFlist.add(temp);
            }
        }

        return String.join("+", DNFlist);
    }


}

