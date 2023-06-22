#include <fstream>
#include <iostream>
#include <string>

using namespace std;

// Prints the change made to the (row, col) position - the change is val
void print_change(int row, int col, int val)
{
    cout << "Fix position " << row << " " << col << " to " << val << endl;
}

// Reads in a grid from a text file given by the filename and
// overwrites it into the grid array
bool read_game(int grid[9][9], const string &fileName)
{
    // Open up the file
    ifstream inFile;
    inFile.open(fileName);

    // If we can open it...
    if (inFile)
    {
        // Iterate through the text file and place each character
        // into a character array
        char ch[9][9];
        for (int i = 0; i < 9; i++)
            for (int j = 0; j < 9; j++)
                inFile >> ch[i][j];

        // Iterate through each character in the array and conver it into
        // a number from 0 to 9, with 0 being a blank tile
        for (int i = 0; i < 9; i++)
            for (int j = 0; j < 9; j++)
                if (ch[i][j] == '-')
                    grid[i][j] = 0;
                else
                    grid[i][j] = ch[i][j] - '0';

        return true;
    }
    else // If we can't open it, simply return
        return false;
}

// Determines the block index given a row and column location
// Imagine that the Sudoku grid of 9 x 9 squares can be grouped into
// 3 x 3 square blocks.  Each block has a row and column location
//   0  1  2   3  4  5  6  7  8
// ----------------------------
// 0        |        |
// 1 (0, 0) | (0, 1) | (0, 2)
// 2        |        |
// ----------------------------
// 3        |        |
// 4 (1, 0) | (1, 1) | (1, 2)
// 5        |        |
// ----------------------------
// 6        |        |
// 7 (2, 0) | (2, 1) | (2, 2)
// 8        |        |
// ----------------------------
// Looking at the grid above, doing row / 3 determines the row location
// of the square block and col / 3 determines the column location of the
// square block.  If you stacked all of the square blocks together
// into a single row, notice that to go to the next row, we must skip
// over three blocks.  This is the convention we need to respect in the
// assignment.  Therefore, 3 * (row / 3) gives us which row we would need to
// access as the single ID. and adding (col / 3) gives us the offset we need
// to get to the right block.
int block_index(int row, int col)
{
    return (row / 3) * 3 + (col / 3);
}

// Function to display the Sudoku grid
void display_grid(int grid[9][9])
{
    // Print out the column information - which column is for what number
    cout << endl;
    cout << "   0 1 2   3 4 5   6 7 8 " << endl;

    // Go through each row
    for (int i = 0; i < 9; i++)
    {
        // If we are at a multiple of three, print out the separator
        if (i % 3 == 0)
            cout << "  +------+-------+------+" << endl;

        // Print out the row number, then the beginning of the row
        cout << i << " |";

        // For each column at this row...
        for (int j = 0; j < 9; j++)
        {
            // If the column is a multiple of 3, print a separator
            if (j == 3 || j == 6)
                cout << "| ";

            // If the grid is 0 or blank, print a dot
            if (grid[i][j] == 0)
                cout << ".";
            else // Else, print the number
                cout << grid[i][j];

            // If we're not at the end of the row, print a space
            // after
            if (j < 8)
                cout << " ";
            else // Print the end of the row
                cout << "|";
        }
        // For the next row
        cout << endl;
    }
    // At the end, print the separator to finish off the bottom of the puzzle
    cout << "  +------+-------+------+" << endl;
    cout << endl;
}

// Checks to see if we can assign a value at (row, col) only if we see
// all other values in the row, column and block of (row, col)
int check_unique(int row, int col, int grid[9][9])
{
    // Used to keep track of the numbers we have seen
    int check[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    // Get out if this is not empty
    if (grid[row][col] != 0)
        return 0;

    // For each row for the target column, determine what the
    // value is and mark it in the tracking array
    for (int i = 0; i < 9; i++)
        check[grid[i][col]] = 1;

    // Do the same for the columns in the target row
    for (int j = 0; j < 9; j++)
        check[grid[row][j]] = 1;

    // Do the same within the block
    // This finds the top left corner of the block
    int begin_row = 3 * (row / 3);
    int begin_col = 3 * (col / 3);

    for (int i = begin_row; i < begin_row + 3; i++)
        for (int j = begin_col; j < begin_col + 3; j++)
            check[grid[i][j]] = 1;


    // If the total number of values we've seen is 8, that means
    // that there is only one value we can write to this spot
    int sum = 0;
    for (int i = 1; i <= 9; i++)
        sum += check[i];

    if (sum == 8)
    {
        // Find that number and return it
        for (int i = 1; i <= 9; i++)
            if (check[i] == 0)
                return i;
    }
    return 0;
}

bool single_candidate_square(int grid[9][9])
{
    cout << "Single candidate square solving: " << endl;
    bool first_hit = false; // Determines if a change has been made
    bool reset = false; // Flag to control when we stop searching

                        // Until there are no more changes...
    while (!reset)
    {
        // Let's propose that we stop
        reset = true;

        // Go through each square and apply check_unique
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                // Skip if non-empty
                if (grid[i][j] != 0)
                    continue;

                // After, apply check_unique.  If the value we get
                // is not an empty value, then modify the grid and
                // print a message
                // We thus make sure that we cycle through the puzzle
                // again until there are no more changes
                int val = check_unique(i, j, grid);
                if (val != 0)
                {
                    print_change(i, j, val);
                    first_hit = true;
                    grid[i][j] = val;
                    reset = false;
                }
            }
        }
    }
    return first_hit;
}

// The question we have to ask is the following
// Given a row and column position, can we write a value here where
// it CANNOT appear elsewhere in this row?
// If we can't, then we write that value here
// If we can, then we shouldn't do anything
bool check_row(int row, int col, int grid[9][9])
{
    // If not empty, get out
    if (grid[row][col] != 0)
        return false;

    // Stores whether we can write a potential number to this spot
    int check[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int count = 0; // Counts how many times we can write a number in this spot

                   // For each potential number
    for (int k = 1; k <= 9; k++)
    {
        bool stop_search = false; // Controls whether we should stop searching
                                  // for this number
                                  // In the end if this is FALSE, then
                                  // the potential number is one we could
                                  // write to the query spot

                                  // First check - Check to see if we have found this number
                                  // within the same row
        for (int j = 0; j < 9; j++)
        {
            // Skip empty squares
            if (grid[row][j] == 0)
                continue;

            // If we have found the number in the row, skip to the next value
            if (grid[row][j] == k)
            {
                stop_search = true;
                break;
            }
        }

        if (stop_search)
            continue;

        // Now, go through each column of this row and do the following
        // For each empty position:
        // (1) Look at all of the rows in the column - did we find the number?
        // (2) Look at all of the values in the block defined by this column
        // location - did we find the number?

        // For any column, if (1) and (2) are both NOT satisfied - that is if
        // we did not see this number in the column and block, that means we
        // can't write the value in the potential spot as we could write
        // it somewhere else
        for (int j = 0; j < 9; j++)
        {
            // Flag that controls if we have seen the value of interest
            // in the column
            bool seen = false;

            // Skip if this is our source column
            if (j == col)
                continue;

            // If this block is not empty, skip
            if (grid[row][j] != 0)
                continue;

            // Search within the rows of this column
            for (int i = 0; i < 9; i++)
            {
                // If we have found this number in the column, stop searching
                if (grid[i][j] == k)
                {
                    seen = true;
                    break;
                }
            }

            // Move to the next column since we've seen the value
            if (seen)
                continue;

            // Now search within the entire block defined by this
            // row and column coordinate
            // We skip the block that belongs to the source coordinate already
            int begin_row = 3 * (row / 3);
            int begin_col = 3 * (j / 3);

            for (int i = begin_row; i < begin_row + 3; i++)
            {
                for (int j = begin_col; j < begin_col + 3; j++)
                {
                    // Skip if it is empty
                    if (grid[i][j] == 0)
                        continue;

                    // Now search to see if we have found this number
                    if (grid[i][j] == k)
                    {
                        // If we have, stop searching
                        seen = true;
                        break;
                    }
                }
            }

            // If we have NOT found the value within the column
            // of interest, then this is not a number we can write
            // at this spot, so signify this by stopping
            // the search
            if (!seen)
            {
                stop_search = true;
                break;
            }
        }

        // If we search all of the columns except the source and
        // we see that there is the value we're searching for at
        // each of them, then there is a potential to write this value in
        // the target location.  Let's record this number
        if (!stop_search)
        {
            check[k] = 1;
            count++;
        }
    }

    // If at the end we see that there is only one possible number
    // that can be written at this spot, do so
    if (count == 1)
    {
        for (int k = 1; k <= 9; k++)
        {
            if (check[k] == 1)
            {
                cout << "Check row. ";
                print_change(row, col, k);
                grid[row][col] = k;
                return true;
            }
        }
    }

    return false;
}

// The question we have to ask is the following
// Given a row and column position, can we write a value here where
// it CANNOT appear elsewhere in this column?
// If we can't, then we write that value here
// If we can, then we shouldn't do anything
bool check_col(int row, int col, int grid[9][9])
{
    // If not empty, get out
    if (grid[row][col] != 0)
        return false;

    // Stores whether we can write a potential number to this spot
    int check[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int count = 0; // Counts how many times we can write a number in this spot

                   // For each potential number
    for (int k = 1; k <= 9; k++)
    {
        bool stop_search = false; // Controls whether we should stop searching
                                  // for this number
                                  // In the end if this is FALSE, then
                                  // the potential number is one we could
                                  // write to the query spot

                                  // First check - Check to see if we have found this number
                                  // within the same column
        for (int i = 0; i < 9; i++)
        {
            // Skip empty squares
            if (grid[i][col] == 0)
                continue;

            // If we have found the number in the column, skip to the next value
            if (grid[i][col] == k)
            {
                stop_search = true;
                break;
            }
        }

        if (stop_search)
            continue;

        // Now, go through each row of this column and do the following
        // For each empty position:
        // (1) Look at all of the rows in the row - did we find the number?
        // (2) Look at all of the values in the block defined by this row
        // location - did we find the number?

        // For any column, if (1) and (2) are both NOT satisfied - that is if
        // we did not see this number in the row and block, that means we
        // can't write the value in the potential spot as we could write
        // it somewhere else
        for (int i = 0; i < 9; i++)
        {
            // Flag that controls if we have seen the value of interest
            // in the row
            bool seen = false;

            // Skip if this is our source row
            if (i == row)
                continue;

            // If this block is not empty, skip
            if (grid[i][col] != 0)
                continue;

            // Search within the columns of this row
            for (int j = 0; j < 9; j++)
            {
                // If we have found this number in the column, stop searching
                if (grid[i][j] == k)
                {
                    seen = true;
                    break;
                }
            }

            // Move to the next column since we've seen the value
            if (seen)
                continue;

            // Now search within the entire block defined by this
            // row and column coordinate
            // We skip the block that belongs to the source coordinate already
            int begin_row = 3 * (i / 3);
            int begin_col = 3 * (col / 3);

            for (int i = begin_row; i < begin_row + 3; i++)
            {
                for (int j = begin_col; j < begin_col + 3; j++)
                {
                    // Skip if it is empty
                    if (grid[i][j] == 0)
                        continue;

                    // Now search to see if we have found this number
                    if (grid[i][j] == k)
                    {
                        // If we have, stop searching
                        seen = true;
                        break;
                    }
                }
            }

            // If we have NOT found the value within the row
            // of interest, then this is not a number we can write
            // at this spot, so signify this by stopping
            // the search
            if (!seen)
            {
                stop_search = true;
                break;
            }
        }

        // If we search all of the rows except the source and
        // we see that there is the value we're searching for at
        // each of them, then there is a potential to write this value in
        // the target location.  Let's record this number
        if (!stop_search)
        {
            check[k] = 1;
            count++;
        }
    }

    // If at the end we see that there is only one possible number
    // that can be written at this spot, do so
    if (count == 1)
    {
        for (int k = 1; k <= 9; k++)
        {
            if (check[k] == 1)
            {
                cout << "Check col. ";
                print_change(row, col, k);
                grid[row][col] = k;
                return true;
            }
        }
    }

    return false;
}

// The question we have to ask is the following
// Given a row and column position, can we write a value here where
// it CANNOT appear elsewhere in this block?
// If we can't, then we write that value here
// If we can, then we shouldn't do anything
bool check_block(int row, int col, int grid[9][9])
{
    // If not empty, get out
    if (grid[row][col] != 0)
        return false;

    // Stores whether we can write a potential number to this spot
    int check[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int count = 0; // Counts how many numbers can be assigned to this spot

    for (int k = 1; k <= 9; k++)
    {
        // Determines if we should stop searching for this number
        bool stop_search = false;

        // First check the block to see if the number matches with
        // any non-zero square
        int begin_row = 3 * (row / 3);
        int begin_col = 3 * (col / 3);
        for (int i = begin_row; i < begin_row + 3; i++)
        {
            for (int j = begin_col; j < begin_col + 3; j++)
            {
                // Skip if empty or the source row and column
                if (grid[i][j] == 0 || (i == row && j == col))
                    continue;

                // If we have found this number, get out
                if (grid[i][j] == k)
                {
                    stop_search = true;
                    break;
                }
            }
        }

        // Move onto the next number if we have found it
        if (stop_search)
            continue;

        // Now check all columns of the query row
        // Same logic as above
        for (int j = 0; j < 9; j++)
        {
            if (grid[row][j] == 0)
                continue;

            if (grid[row][j] == k)
            {
                stop_search = true;
                break;
            }
        }

        if (stop_search)
            continue;

        // Check all of the rows in the column
        // Same logic as above
        for (int i = 0; i < 9; i++)
        {
            if (grid[i][col] == 0)
                continue;

            if (grid[i][col] == k)
            {
                stop_search = true;
                break;
            }
        }

        // When we get to this point, that means that we have
        // not found this value in the row, column and block so
        // this is a number that could go here.  Remember that for the end.
        if (!stop_search)
        {
            check[k] = 1;
            count++;
        }
    }

    // If at the end we see that there is only one possible number
    // that can be written at this spot, do so.  Same logic as before.
    if (count == 1)
    {
        for (int k = 1; k <= 9; k++)
        {
            if (check[k] == 1)
            {
                cout << "Check block. ";
                print_change(row, col, k);
                grid[row][col] = k;
                return true;
            }
        }
    }

    return false;
}

// Applies check_row, check_col and check_grid to every square
// from top left to bottom right.  Keep doing this until there are
// no more changes
bool single_square_candidate(int grid[9][9])
{
    cout << "Single square candidate solving: " << endl;

    // Same logic as before.  Instead of using check_unique, we use
    // check_row, check_col and check_block
    bool first_hit = false;
    bool check = false;
    while (!check)
    {
        check = true;
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                if (check_row(i, j, grid))
                {
                    first_hit = true;
                    check = false;
                    continue; // We continue here because the square has been
                              // modified so we don't need to use the other
                              // checks.
                }

                if (check_col(i, j, grid))
                {
                    first_hit = true;
                    check = false;
                    continue;
                }

                if (check_block(i, j, grid))
                {
                    first_hit = true;
                    check = false;
                    continue;
                }
            }
        }
    }
    return first_hit;
}

// Solves the Sudoku puzzle using the functions above
void solve_sudoku(int grid[9][9])
{
    // Keep iterating, alternating the use of single_candidate_square and
    // single_square_candidate until there are no more changes
    while (true)
    {
        display_grid(grid);
        bool check1 = single_candidate_square(grid);
        if (!check1)
            cout << "... No Change" << endl;
        cout << endl;

        display_grid(grid);
        bool check2 = single_square_candidate(grid);
        if (!check2)
            cout << "... No Change" << endl;
        cout << endl;

        if (!check1 && !check2)
            break;
    }
}

int main(int argc, char *argv[])
{
    string filename;
    cout << "Enter in a filename: ";
    cin >> filename;

    int grid[9][9];
    if (read_game(grid, filename))
        solve_sudoku(grid);
    else
        cout << "File cannot be opened... Bye!" << endl;
}
