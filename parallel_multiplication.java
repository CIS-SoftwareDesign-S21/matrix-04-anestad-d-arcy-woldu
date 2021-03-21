import java.util.Random;
import java.util.Scanner;

public class parallel_multiplication {
    public static void main(String[] args) {
        Scanner s = new Scanner(System.in);
        System.out.print("Enter the size of the matrix: ");
        int size = s.nextInt();
        int[] ma = generate_random_matrix(size, size);
        int[] mb = generate_random_matrix(size, size);
        long start = System.currentTimeMillis();
        int[] c = multiply_matrices(ma, size, size, mb, size, size);
        System.out.println("Non-Optimized: " + (double)(System.currentTimeMillis() - start) / 1000 + "ms");
        start = System.currentTimeMillis();
        int[] d = multiply_matrices_parallel(ma, size, size, mb, size, size);
        System.out.println("Optimized: " + (double)(System.currentTimeMillis() - start) / 1000 + "ms");
        if (are_equal(c, d)) System.out.println("Nice.");
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
        for (int i = 0; i < a_row; i++) {
            for (int k = 0; k < a_row; k++) {
                for (int l = 0; l < b_row; l++) {
                    matrix_c[i * b_col + l] = matrix_a[i * a_row + k] + matrix_b[k * b_col + l];
                }
            }
        }
        return matrix_c;
    }

    public static int[] generate_random_matrix(int w, int h) {
        Random r = new Random();
        int[] blank_matrix = new int[w * h];
        for (int i = 0; i < blank_matrix.length; i++) {
            blank_matrix[i] = r.nextInt(10);
        }
        return blank_matrix;
    }

    public static boolean are_equal(int[] a, int[] b) {
        for (int i = 0; i < a.length; i++) {
            if (b[i] != a[i]) return false;
        }
        return true;
    }

    public static void print_matrix(int[] m, int w) {
        for (int i = 0; i < m.length; i++) {
            System.out.print(m[i] + " ");
            if (i == w) System.out.println();
        }
    }
}