package string;

import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class expressionModifier {

    public static String modify(String input){
        if (input.contains("+1+")){
            input = "1";
        }
        if (input.length() > 1 && ((input.charAt(0) == '1' && input.charAt(1) == '+') || (input.charAt(input.length() - 1) == '1' && input.charAt(input.length() - 2) == '+'))){
            input = "1";
        }
        else {
            if (input.length() > 1 && input.contains("1")) {
                input = input.replace("1", "");
            }
        }
        if (input.equals("")){
            input = "0";
        }
        while (input.length() > 1){
            if (input.contains("11")){
                input = input.replace("11", "1");
            }
            else {
                break;
            }
        }
        List<String> list = new ArrayList<String>();
        String[] split = input.split("\\+");
        for (String i: split){
            if (!list.contains(i)){
                list.add(i);
            }
        }
        input = String.join("+", list);
        if (input.length() < 5){
            input = expressionChecker.check(input);
        }
        return input;
    }
    public static String check(String dnf){
        String[] inputList = dnf.split("\\+");
        String pattern = "!(\\p{Upper})\\p{Upper}*\\1";
        String pattern2 = "(\\p{Upper})[\\p{Upper}]*!(?=.*\\1)";
        Pattern regex = Pattern.compile(pattern);
        Pattern regex2 = Pattern.compile(pattern2);
        Matcher matcher, matcher2;
        ArrayList<String> finalList = new ArrayList<String>();
        for (String i: inputList){
            matcher = regex.matcher(i);
            matcher2 = regex2.matcher(i);
            if (!matcher.find() && !matcher2.find()){
               finalList.add(i);
            }
        }
        return String.join("+", finalList);
    }
}
