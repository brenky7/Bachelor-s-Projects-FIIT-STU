import java.util.*;

public class Main{
    static class Node{
        private int[][] puzzle;
        private int pocetTahov;
        private int priorita;
        private Node next;
        private String operand;
        public Node(int[][] puzzle, int pocetTahov, int priorita) {
            this.puzzle = puzzle;
            this.pocetTahov = pocetTahov;
            this.priorita = priorita;
        }
        public String getOperand(){
            return operand;
        }
        public void setOperand(String operand){
            this.operand = operand;
        }
        public int[][] getPuzzle(){
            return puzzle;
        }
        public int getPocetTahov(){
            return pocetTahov;
        }
        public int getPriorita(){
            return priorita;
        }
        public void zmenPrioritu(){    //zmeni prioritu na nahodne cislo v rozmedzi 1 a povodnej priority
            this.priorita = new Random().nextInt(Math.abs(this.getPriorita()*2))+1;
        }
    }
    static final int[][] tahy ={      //matica moznych pohybov
            {1, 0},
            {-1, 0},
            {0, 1},
            {0, -1}
    };
    private static int[] getSuradnice(int[][] puzzle, int n, int cislo){ //vrati suradnice cisla v poli
        int[] suradnice = new int[2];
        for(int i = 0; i < n; i++){
            for(int j = 0; j < n; j++){
                if(puzzle[i][j] == cislo){
                    suradnice[0] = i;
                    suradnice[1] = j;
                    break;
                }
            }
        }
        return suradnice;
    }
    public static boolean checkSolvability(int[][] puzzle, int[][] goal, int n){  //kontrola ci je puzzle mozne vyriesit
        int inv1 = 0, inv2 = 0;
        int[] puzzle1D = new int[n*n];
        int[] goal1D = new int[n*n];
        int k = 0;
        for (int i = 0; i < n; i++){            //prevedie 2D pole na 1D
            for (int j = 0; j < n; j++, k++){
                puzzle1D[k] = puzzle[i][j];
                goal1D[k] = goal[i][j];
            }
        }
        for (int i = 0; i < n*n; i++){              //spocita inverzie
            for (int j = i+1; j < n*n; j++){
                if (puzzle1D[i] != 0 & puzzle1D[j] != 0 & puzzle1D[i] > puzzle1D[j]){
                    inv1++;
                }
                if (goal1D[i] != 0 & goal1D[j] != 0 & goal1D[i] > goal1D[j]){
                    inv2++;
                }
            }
        }
        if (n % 2 == 1){     //ak je rozmer pola neparny, tak je riesitelne ak pocty inverzii maju rovnaku paritu
            return inv1 % 2 == inv2 % 2;
        }
        else{           // ak je parny, tak je riesitelne ak parita inverzii a parita riadku blank su rovnake
            int riadokPuzzle = n - getSuradnice(puzzle, n, 0)[0];
            int riadokGoal = n - getSuradnice(goal, n, 0)[0];
            if (riadokPuzzle % 2 == riadokGoal % 2 && inv1 % 2 == inv2 % 2){
                return true;
            }
            else return riadokPuzzle % 2 != riadokGoal % 2 && inv1 % 2 != inv2 % 2;
        }
    }
    public static int[][][] vyrobPuzzle(int n){
        int[][] puzzle = new int[n][n];
        int[][] goal = new int[n][n];
        List<Integer> cisla = new ArrayList<>();
        List<Integer> cisla2 = new ArrayList<>();
        for (int i = 0; i < n*n; i++){
            cisla.add(i);
            cisla2.add(i);
        }
        Collections.shuffle(cisla);
        Collections.shuffle(cisla2);
        int index = 0;
        for (int i = 0; i < n; i++){
            for (int j = 0; j < n; j++, index++){
                puzzle[i][j] = cisla.get(index);
                goal[i][j] = cisla2.get(index);
            }
        }
        int[][][] vyrob = new int[2][n][n];
        vyrob[0] = puzzle;
        vyrob[1] = goal;
        return vyrob;
    }
    public static void main(String[] args){
        Scanner sc = new Scanner(System.in);
        System.out.println("Zadajte m pre manualny vstup alebo a pre automaticky test:");
        char vstup = sc.nextLine().charAt(0);
        if (vstup == 'a'){
            System.out.println("Zadajte rozmer pola n (n>=2):");
            int n = Integer.parseInt(sc.nextLine());
            System.out.println("Zadajte pocet testov:");
            int pocetTestov = Integer.parseInt(sc.nextLine());
            int[][][][] testy = new int[pocetTestov][2][n][n];
            for (int i = 0; i < pocetTestov; i++){
                testy[i] = vyrobPuzzle(n);
                while(!checkSolvability(testy[i][0], testy[i][1], n)){
                    testy[i] = vyrobPuzzle(n);
                }
            }
            /*long zaciatok = System.nanoTime();
            for (int i = 0; i < pocetTestov; i++){
                boolean solved = false;
                while (!solved){                //opakuje sa kym sa nenajde riesenie
                    solved = greedyAlgoritmus(testy[i][0], testy[i][1], n, 1);
                }
                System.out.println("Test " + (i+1));
            }
            long stred = System.nanoTime();
            for (int i = 0; i < pocetTestov; i++){
                boolean solved = false;
                while (!solved){                //opakuje sa kym sa nenajde riesenie
                    solved = greedyAlgoritmus(testy[i][0], testy[i][1], n, 2);
                }
                System.out.println("Test " + (i+1));
            }
            long koniec = System.nanoTime();
            System.out.println("Cas vykonania h1: " + (stred - zaciatok)/1000000 + " ms");
            System.out.println("Cas vykonania h2: " + (koniec - stred)/1000000 + " ms");*/
            System.out.println("Zadajte heuristiku (1 alebo 2):");
            int heuristika = Integer.parseInt(sc.nextLine());
            long zaciatok = System.nanoTime();
            for (int i = 0; i < pocetTestov; i++){
                boolean solved = false;
                while (!solved){                //opakuje sa kym sa nenajde riesenie
                    solved = greedyAlgoritmus(testy[i][0], testy[i][1], n, heuristika);
                }
                System.out.println("Test " + (i+1));
            }
            long koniec = System.nanoTime();
            System.out.println("Cas vykonania: " + (koniec - zaciatok)/1000000 + " ms");
        }
        else if (vstup == 'm'){
            System.out.println("Zadajte rozmer pola n:");
            int n = Integer.parseInt(sc.nextLine());
            System.out.println("Zadajte hracie pole po riadkoch, '0' reprezentuje blank:");
            int[][] puzzle = new int[n][n];
            int[][] goal = new int[n][n];
            for (int i = 0; i < n; i++) {
                String[] line = sc.nextLine().split(" ");
                for (int j = 0; j < n; j++) {
                    puzzle[i][j] = Integer.parseInt(line[j]);
                }
            }
            System.out.println("Zadajte cielove pole po riadkoch, '0' reprezentuje blank:");
            for (int i = 0; i < n; i++) {
                String[] line = sc.nextLine().split(" ");
                for (int j = 0; j < n; j++) {
                    goal[i][j] = Integer.parseInt(line[j]);
                }
            }
            System.out.println("Zadajte heuristiku (1 alebo 2):");
            int heuristika = Integer.parseInt(sc.nextLine());
            if (n < 2){
                System.out.println("Nespravny rozmer pola.");
            }
            else if (checkSolvability(puzzle, goal, n)){
                System.out.println("Puzzle je mozne vyriesit");
                boolean solved = false;
                while (!solved){                //opakuje sa kym sa nenajde riesenie
                    if (heuristika == 1) {
                        solved = greedyAlgoritmus(puzzle, goal, n, heuristika);
                    }
                    else {
                        solved = greedyAlgoritmus(puzzle, goal, n, heuristika);
                    }
                }
            }
            else {
                System.out.println("Puzzle nie je mozne vyriesit :((");
            }
        }
        else{
            System.out.println("Nespravny vstup.");
        }
    }
    public static boolean checkPozicia(int x, int y, int n){   //kontrola ci je pozicia v poli
        if (x >= 0 && x < n && y >= 0 && y < n){
            return true;
        }
        else{
            return false;
        }
    }
    public static int[][] deepCopy(int[][] puzzle, int n){      //vytvori nove pole a skopiruje do neho stare
        int[][] copy = new int[n][n];
        for (int i = 0; i < n; i++){
            for (int j = 0; j < n; j++){
                copy[i][j] = puzzle[i][j];
            }
        }
        return copy;
    }
    public static void vymen(int[][] puzzle, int x, int y, int x2, int y2){  // vykonava pohyb policok
        int temp = puzzle[x][y];
        puzzle[x][y] = puzzle[x2][y2];
        puzzle[x2][y2] = temp;
    }
    public static void printPuzzle(int[][] puzzle, int n){  //vypise puzzle
        String output = "";
        for (int i = 0; i < n; i++){
            for (int j = 0; j < n; j++){
                output += puzzle[i][j];
                output += " ";
            }
            output += "\n";
        }
        System.out.println(output);
    }
    public static void printCesta(Node aktual, int n){      //vypise cestu od prveho stavu po cielovy
        if (aktual == null){
            return;
        }
        System.out.println(aktual.getOperand());
        printPuzzle(aktual.getPuzzle(), n);
        printCesta(aktual.next, n);
    }
    private static int heuristika1(int[][] puzzle, int[][] goal, int n){   //pocet policok na nespravnom mieste
        int heuristika = 0;
        for(int i = 0; i < n; i++){
            for(int j = 0; j < n; j++){
                if (puzzle[i][j] != 0 & puzzle[i][j] != goal[i][j]){
                    heuristika += 1;
                }
            }
        }
        return heuristika;
    }
    private static int heuristika2(int[][] puzzle, int[][] goal, int n){  //sucet vzdialenosti policok od cielovej pozicie
        int heuristika = 0;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                int policko = puzzle[i][j];
                if (policko != 0) {
                    int[] suradnice = getSuradnice(goal, n, policko);
                    heuristika += Math.abs(i - suradnice[0]) + Math.abs(j - suradnice[1]);
                }
            }
        }
        return heuristika;
    }
    private static boolean greedyAlgoritmus(int[][] puzzle, int[][] goal, int n, int heuristika){
        PriorityQueue<Node> stavy = new PriorityQueue<>(Comparator.comparingInt(Node::getPriorita));
        Set<String> vybaveneStavy = new HashSet<>();
        Node prvyNode = new Node(puzzle, 0, 1);
        prvyNode.setOperand("Riesenie: ");
        stavy.add(prvyNode);            //vytvory prvy node a prida ho do priority queue
        while (!stavy.isEmpty()) {
            Node aktualnyNode = stavy.poll();   //vyberie node s najmensou prioritou
            stavy.clear();                      //vymaze zvysok priority queue
            int[][] aktualnePuzzle = aktualnyNode.getPuzzle();
            if (Arrays.deepEquals(aktualnePuzzle, goal)) {      //ak je aktualny stav cielovy, tak vypise cestu
                printCesta(prvyNode, n);
                System.out.println("Pocet tahov: " + aktualnyNode.getPocetTahov());
                return true;
            }
            vybaveneStavy.add(Arrays.deepToString(aktualnePuzzle));     //prida aktualny stav do mnoziny vybavenych stavov
            int[] xyNuly = getSuradnice(aktualnePuzzle, n, 0);
            for (int[] move : tahy) {       //iteruje cez vsetky mozne pohyby
                int x = xyNuly[0] + move[0];
                int y = xyNuly[1] + move[1];
                if (checkPozicia(x, y, n)) {    //ak je pohyb mozny, tak vytvori nove pole
                    int[][] novePuzzle = deepCopy(aktualnePuzzle, n);
                    vymen(novePuzzle, xyNuly[0], xyNuly[1], x, y);
                    if (!vybaveneStavy.contains(Arrays.deepToString(novePuzzle))) {   //ak sa stav nenachadza v mnozine vybavenych stavov, tak ho prida do priority queue
                        int priorita;
                        if (heuristika == 1){
                            priorita = heuristika1(novePuzzle, goal, n);
                        }
                        else{
                            priorita = heuristika2(novePuzzle, goal, n);
                        }
                        Node newNode = new Node(novePuzzle, aktualnyNode.getPocetTahov() + 1, priorita);
                        if (move == tahy[0]) {
                            newNode.setOperand("Hore:");
                        }
                        if (move == tahy[1]) {
                            newNode.setOperand("Dole:");
                        }
                        if (move == tahy[2]) {
                            newNode.setOperand("Vlavo:");
                        }
                        if (move == tahy[3]) {
                            newNode.setOperand("Vpravo:");
                        }
                        if (!stavy.isEmpty() && newNode.getPriorita() == stavy.peek().getPriorita()) {
                            newNode.zmenPrioritu();     //ak uz niektory z moznych stavov ma rovnaku prioritu, aktualna sa zmeni
                        }
                        stavy.add(newNode);
                    }
                }
            }
            aktualnyNode.next = stavy.peek();   //prida child pointer na najlepsi mozny stav
        }
        return false;
    }
}