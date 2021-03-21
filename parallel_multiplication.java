import java.util.Random;
import java.util.concurrent.Semaphore;

public class parallel_multiplication {
    public static void main(String[] args) {
        long start = 0;
        double c_time = 0;
        double d_time = 0;
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
            if (!are_equal(c, d)) {
                System.out.println("Calculated matrices were not the same. Terminating program...");
                break;
            }
            System.out.println(c_time + ", " + d_time);
        }
    }

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

    public static int[] multiply_matrices_parallel(int[] matrix_a, int a_row, int a_col, int[] matrix_b, int b_row, int b_col) {
        int[] matrix_c = new int[a_row * b_col];
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
        long i = System.currentTimeMillis();
        while (System.currentTimeMillis() - i < 100);
        return matrix_c;
    }

    public static int[] generate_random_matrix(int w, int h) {
        Random r = new Random();
        int[] blank_matrix = new int[w * h];
        for (int i = 0; i < blank_matrix.length; i++) {
            blank_matrix[i] = r.nextInt();
        }
        return blank_matrix;
    }

    public static boolean are_equal(int[] a, int[] b) {
        if (a.length != b.length) return false;
        for (int i = 0; i < a.length; i++) {
            if (b[i] != a[i]) return false;
        }
        return true;
    }
}