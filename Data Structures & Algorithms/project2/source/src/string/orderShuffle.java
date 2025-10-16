package string;

public class orderShuffle {
    public static String[] finalShuffle;
    public static String[] shuffle(String order){
        finalShuffle = new String[order.length()];
        char[] orderInChar = order.toCharArray();
        char[] shuffled = new char[orderInChar.length];
        for (int j = 0; j < orderInChar.length; j++) {
            for (int i = 0; i < orderInChar.length; i++) {
                shuffled[i] = orderInChar[ (i + 1)%orderInChar.length ];
            }
            finalShuffle[j] = String.valueOf(shuffled);
            orderInChar = shuffled;
            shuffled = new char[orderInChar.length];
        }
        return finalShuffle;
    }

}
