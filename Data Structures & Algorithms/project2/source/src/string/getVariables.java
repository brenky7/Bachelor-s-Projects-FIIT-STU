package string;

public class getVariables {
    public static String get(String input){
        String[] variables = new String[0];
        for (int i = 0; i < input.length(); i++){
            if (Character.isLetter(input.charAt(i))){
                String[] temp = new String[variables.length + 1];
                for (int j = 0; j < variables.length; j++){
                    temp[j] = variables[j];
                }
                boolean found = false;
                for (int k = 0; k < variables.length; k++){
                    if (variables[k].equals(String.valueOf(input.charAt(i)))){
                        found = true;
                        break;
                    }
                }
                if (!found){
                    temp[variables.length] = String.valueOf(input.charAt(i));
                    variables = temp;
                }
            }
        }
        return String.join("", variables);
    }
}
