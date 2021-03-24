import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;

public class par_mult_testing {
    /*
    Creates two randomly generated matrices, which will most likely be different, then checks to see
    if they are not the same object (completely equal)
     */
    @Test
    void shouldProduceTwoUniqueMatrices() {
        int width = 10, height = 10;
        int[] matrix_one = parallel_multiplication.generate_random_matrix(width, height);
        int[] matrix_two = parallel_multiplication.generate_random_matrix(width, height);
        // Statistically speaking, these two matrices have a 1/2^100 chance of being the same
        Assertions.assertNotSame(matrix_one, matrix_two);
        // While not entirely trustworthy, creating two randomly generated matrices will usually result in different matrices
    }

    /*
    Creates two equal matrices using different methods then checks to see if they are the same.
    Then checks to see if two different matrices are seen to be different matrices.
     */
    @Test
    void shouldCorrectlyCompareMatrices() {
        int[] matrix_one = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        int[] matrix_two = new int[9];
        for (int i = 0; i < 9; i++) matrix_two[i] = i + 1;
        Assertions.assertTrue(parallel_multiplication.are_equal(matrix_one, matrix_two));
        matrix_two[0] = 10;
        Assertions.assertFalse(parallel_multiplication.are_equal(matrix_one, matrix_two));
    }

    /*
    Tests to see if the matrix_multiplication function correctly multiplies two matrices together.
    The solved_matrix is the matrix that is calculated outside of the program using matrix_one and matrix_two.
    The output of matrix_one and matrix_two are then compared to the solved_matrix.
     */
    @Test
    void shouldProperlyMultiplyMatrices() {
        int[] solved_matrix = {2, 3, 4, 5};
        int[] matrix_one = {1, 0, 0, 1};
        int[] matrix_two = {2, 3, 4, 5};
        Assertions.assertArrayEquals(parallel_multiplication.multiply_matrices(matrix_one, 2, 2, matrix_two, 2, 2), solved_matrix);
    }

    /*
    Tests to see if the matrix_multiplication_parallel function correctly multiplies two matrices together.
    The solved_matrix is the matrix that is calculated outside of the program using matrix_one and matrix_two.
    The output of matrix_one and matrix_two are then compared to the solved_matrix.
    This test only differs from the above test in that it uses the optimized multiplication function.
    This test is also usually longer than the others due to the creation and handling of threads.
     */
    @Test
    void shouldProperlyMultiplyMatricesUsingThreads() {
        int[] solved_matrix = {2, 3, 4, 5};
        int[] matrix_one = {1, 0, 0, 1};
        int[] matrix_two = {2, 3, 4, 5};
        Assertions.assertArrayEquals(parallel_multiplication.multiply_matrices_parallel(matrix_one, 2, 2, matrix_two, 2, 2), solved_matrix);
    }
}