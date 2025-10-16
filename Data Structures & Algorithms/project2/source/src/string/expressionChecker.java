package string;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class expressionChecker {
    public static String check(String input){
        String pattern = "^![A-Z]\\+[A-Z]$";
        String pattern2 = "^[A-Z]\\+![A-Z]$";
        Pattern regex = Pattern.compile(pattern);
        Pattern regex2 = Pattern.compile(pattern2);
        Matcher matcher = regex.matcher(input);
        Matcher matcher2 = regex2.matcher(input);
        if (matcher.find() || matcher2.find()){
            return "1";
        }
        else {
            return input;
        }
    }
}
