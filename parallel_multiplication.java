import java.util.Random;
import java.util.concurrent.Semaphore;

public class parallel_multiplication {
    public static void main(String[] args) {
        long start = 0;
        double c_time = 0;
        double d_time = 0;
        // iteration is used to produce enough data to graph and analyze
        for (int i = 0; i < 12; i++) {
            int s = i * 100;
            int[] a = generate_random_matrix(s, s);
            int[] b = generate_random_matrix(s, s);
            start = System.currentTimeMillis();
            int[] c = multiply_matrices_parallel(a, s, s, b, s, s);
            c_time = (double) ((System.currentTimeMillis() - start) - 100)/ 1000;
            start = System.currentTimeMillis();
            int[] d = multiply_matrices(a, s, s, b, s, s);
            d_time = (double) (System.currentTimeMillis() - start) / 1000;
            // Checks to see if the two calculated matrices are the same. If they are not, there is a problem with the program and the program will terminate.
            if (!are_equal(c, d)) {
                System.out.println("Calculated matrices were not the same. Terminating program...");
                break;
            }
            System.out.println(c_time + ", " + d_time);
        }
    }

    /*
    Accepts array of integers followed by two integers which are the first matrix and the first matrix's row and column values.
    Then accepts another group of an array of integers and two integers that represent the second matrix and its row and column values.
    The function creates a blank integer array to store the product in. The algorithm iterates through the columns in the
    first matrix and the rows in the second matrix. Then the values are multiplied together and stored within the blank matrix
    that has been allocated to store all the required values. This function takes O(n^3) time to complete.
     */
    public static int[] multiply_matrices(int[] matrix_a, int a_row, int a_col, int[] matrix_b, int b_row, int b_col) {
        int[] matrix_c = new int[a_row * b_col];
        for (int i = 0; i < a_row; i++) {
            for (int j = 0; j < b_col; j++) {
                for (int k = 0; k < a_row; k++) {
                    matrix_c[i * b_col + j] += matrix_a[i * a_row + k] * matrix_b[k * b_col + j];
                }
            }
        }
        return matrix_c;
    }

    /*
    Accepts array of integers followed by two integers which are the first matrix and the first matrix's row and column values.
    Then accepts another group of an array of integers and two integers that represent the second matrix and its row and column values.
    The function creates a blank integer array to store the product in. The algorithm iterates through the columns in the
    first matrix. The program then creates a thread which will handle the second matrix iterations and the multiplication/addition
    operations that will be stored within the blank matrix. In theory, this algorithm takes n^2 + n time to complete because the program
    will run multiple instances of the inner iteration. The "+ n", which is then omitted from the Big-O, comes from the combination
    of the first and last thread ran that does not run with other threads. The time complexity of this algorithm is O(n^2).
     */
    public static int[] multiply_matrices_parallel(int[] matrix_a, int a_row, int a_col, int[] matrix_b, int b_row, int b_col) {
        int[] matrix_c = new int[a_row * b_col];
        /* A semaphore is used to ensure that no more than 4 threads are running at the same time. This is to help the program
        produce similar results regardless of the machine that is running it.
        */
        Semaphore sem = new Semaphore(4);
        for (int i = 0; i < a_row; i++) {
            int finalI = i;
            Thread t = new Thread(() -> {
                try {
                    sem.acquire();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                for (int k = 0; k < a_row; k++) {
                    for (int l = 0; l < b_row; l++) {
                        matrix_c[finalI * b_col + l] += matrix_a[finalI * a_row + k] * matrix_b[k * b_col + l];
                    }
                }
                sem.release();
            });
            t.start();
        }
        /* Below line is in substitute for a join() implementation. The resources used on the machine this was originally
        written on produce unexplained changes in the data which will lead to having a join() implementation result in an
        outcome that is based on the original machine's current workload, external to the program. The addition of 100ms
        to the recorded time simply holds the function until all threads are most likely complete. The extra 100ms is then
        subtracted from the final time after the algorithm has been completed.
        */
        long i = System.currentTimeMillis();
        while (System.currentTimeMillis() - i < 100);
        return matrix_c;
    }

    /*
    Generates an array of random integers, either 1 or 0, that represent a matrix of size 'w by h'
     */
    public static int[] generate_random_matrix(int w, int h) {
        Random r = new Random();
        int[] blank_matrix = new int[w * h];
        for (int i = 0; i < blank_matrix.length; i++) {
            blank_matrix[i] = r.nextInt();
        }
        return blank_matrix;
    }

    /*
    Checks to see whether the two provided matrices are equal or not. This function is used to test the functionality of the
    program during runtime.
     */
    public static boolean are_equal(int[] a, int[] b) {
        if (a.length != b.length) return false;
        for (int i = 0; i < a.length; i++) {
            if (b[i] != a[i]) return false;
        }
        return true;
    }
}