#include <cstdio>
#include <cassert>

//Infix expressions are generally more complicated for a computer to process - primarily due to parentheses.  
//Dijkstra's Shunting Yard algorithm converts an infix expression to postfix (Polish notation) to allow for simple
//calculation using a stack.  I had begun writing this algorithm as I remembered that polish notation is easier, but 
//I could not see a way to preserve the "no alloc" constraint or employ recursion.
//Another method is to recursively parse the infix expression until a parenthesis is reached, which would signify another "sub-expression"
//which could be parsed in the same way recursively.
//This is called a Recursive Descent Parser.
//     expression
//|------------------|
//3 + 4 - (3 * 3) / 4
//        |_____|
//        sub-expr
//
//Time complexity: O(n)  This should be a O(n) algorithm where n = string length.  
//Space complexity: O(1) as no additional data is created with the exception of a few local primitives. 
//                  The functions on the stack will vary depending on how many operators and parenthesized expressions
//                  there are.


//Function declarations
float tokenizeNumbers(const char*& eq, int& countParenthesis);

float tokenizeExpression(const char*& eq, int& countParenthesis);

float tokenizeMulDiv(const char*& eq, int& countParenthesis);

//NAME: isDigit
//DESCRIPTION:  Checks to see if a character is between 0 and 9.
//INPUT: 
//    c - The character to check.
//OUTPUT:
//    none
//RETURNS:
//    True if the character falls between 0 and 9.
bool isDigit(char c)
{
    return ((c >= '0') && (c <= '9'));
}

//NAME: toDigit
//DESCRIPTION:  Converts a character to its integer equivalent.
//INPUT: 
//    c - The character to check.
//OUTPUT:
//    none
//RETURNS:
//    The integer equivalent of c.
int toDigit(char c)
{
    //I believe ASCII goes from 48 to 57 for 0 - 9.
    return c - '0';
}

//NAME: isOperator
//DESCRIPTION:  Checks to see if a character is an operator.
//INPUT: 
//    c - The character to check.
//OUTPUT:
//    none
//RETURNS:
//    True if the character is any of the four operators.
bool isOperator(char c)
{
    static char operators[] = { '+', '-' , '/', '*' };
    const int maxOperators = 4;

    for (int i = 0; i < maxOperators; i++)
        if (c == operators[i])
            return true;

    return false;
}

//NAME: makeFloat
//DESCRIPTION:  Converts a string to a float.
//              Starts scanning from eq until a non-digit character.
//              Performs the same function as atof.
//INPUT: 
//    eq  - The string to convert.
//OUTPUT:
//    end - This pointer marks where the scan stopped.  It should not be a digit.
//RETURNS:
//    The float representation of the string.
float makeFloat(const char* eq, const char*& end)
{
    //Make sure that the string is non-NULL.
    assert(eq != NULL);

    //The mantissa.
    float beforeDecimal = 0.0;

    //The fractional part.
    float afterDecimal = 0.0;

    //We use this value to determine by how much the fraction
    //should be divided by ten.
    int divisor = 1;

    //The number could be negative.
    int sign = 1;

    bool isFraction = false;

    //If the first character is a negative then we have a negative number.
    if (*eq == '-')
    {
        sign = -1;
        eq++;
    }

    //Continue scanning until we reach a character that isn't a digit
    //or decimal point.
    while (isDigit(*eq) || *eq == '.')
    {
        //If this character is a digit, are we before or after
        //the decimal point?
        if (isDigit(*eq))
        {
            if (isFraction)
            {
                //This is the fractional part after the decimal.
                afterDecimal = (afterDecimal * 10) + toDigit(*eq);
                divisor = divisor * 10;
            }
            else
            {
                //This is the mantissa.
                beforeDecimal = (beforeDecimal * 10) + toDigit(*eq);
            }
        }
        else if (*eq == '.')
        {
            //We've found a decimal point, but this might not be the first we've found.
            if (isFraction)
            {
                //We've found a decimal point but are already in the fractional part, 
                //so we are dealing with a string like so: 12.3.5
                //  And we are located here:                   ^
                end = eq;

                //Update the end pointer and return the value.
                return sign * (beforeDecimal + (afterDecimal / divisor));
            }
            else
                isFraction = true;
        }

        eq++;
    }

    //Update the end pointer and return the value.
    end = eq;
    return sign * (beforeDecimal + (afterDecimal / divisor));
}

//NAME: tokenizeNumbers
//DESCRIPTION:  By order of operations, the lowest possible sub-expression to be parsed
//              is one in parenthesis.  We will assume that even a number by itself is 
//              surrounded by parentheses: (5) + (6) + (4 - 3)
//              This function will therefore look for any numbers.  When it finds a parentheses
//              it will evaluate that expression until it boils it down to just one number,
//              and then we're back here.
//INPUT/OUTPUT:
//    eq  - The pointer to where we currently are in the expression.
//    countParenthesis - The counter which keeps track of how many parentheses we've come across.
//                       Should be back to zero when the expression is done.
//RETURNS:
//    The float value representing the value in parentheses.
float tokenizeNumbers(const char*& eq, int& countParenthesis)
{
    //Border condition check
    assert(eq != NULL);

    //Spaces don't matter to us
    while (*eq == ' ')
        eq++;

    //So at this point we are sitting here in our hypothetical expression
    //-(4+6)
    //^
    bool hasNegative = false;
    if (*eq == '-')
    {
        hasNegative = true;
        eq++;
    }

    //We've found an open parenthesis!  This means we can recursively parse
    //another expression in exactly the same way we've been doing so far.
    if (*eq == '(')
    {
        eq++;
        countParenthesis++;

        float calculated = tokenizeExpression(eq, countParenthesis);
        assert(*eq == ')'); //Tokenize parentheses should always leave off at the closing parenthesis!

        eq++;
        countParenthesis--;

        if (hasNegative)
            return calculated * -1;
        else
            return calculated;
    }

    //Convert the character string to a float, and then update the current position
    //in our string to be after this float.
    const char* eptr;
    float toNumber = makeFloat(eq, eptr);

    //It would be bad if the float value was zero characters long.  This is 
    //a very unexpected error condition.
    assert(eptr != eq);

    //Update the current pointer of our expression to the end pointer after make float.
    eq = eptr;

    if (hasNegative)
        return toNumber * -1;
    else
        return toNumber;
}

//NAME: tokenizeMulDiv
//DESCRIPTION:  Multiplication and division have higher priority than addition or subtraction.  
//              We should therefore look at these first.  The only thing with higher priority
//              are parentheses.  We consider all numbers to be surrounded by parentheses as well, 
//              this way we can retrieve numbers before the operator that applies to them.
//INPUT/OUTPUT:
//    eq  - The pointer to where we currently are in the expression.
//    countParenthesis - The counter which keeps track of how many parentheses we've come across.
//                       Should be back to zero when the expression is done.
//RETURNS:
//    The float value representing the value after a multiplication or division.
float tokenizeMulDiv(const char*& eq, int& countParenthesis)
{
    //Always extract numbers or sub-expressions first!
    float first = tokenizeNumbers(eq, countParenthesis);

    //It's ok to loop infinitely when he have a definitive out in the loop.
    while (true)
    {
        //Ignore spaces.
        while (*eq == ' ')
            eq++;

        //Let's look at an operator.  
        //If eq isn't pointing at a division or multiplication symbol
        //there is nothing for us to do here, return the number from tokenizeNumbers.
        char opr = *eq;
        if (opr != '*' && opr != '/')
            return first;

        eq++;

        //If we've gotten this far it means we must be either dividing or multiplying,
        //so let's get the second number and figure out what to do.
        //Again this function 'tokenizeNumbers' will either evaluate a set of parentheses
        //or directly give us the number if there are no parentheses.
        float second = tokenizeNumbers(eq, countParenthesis);

        if (opr != '/')
        {
            first = first * second;
        }
        else
        {
            //Division by zero is a bad thing.
            assert(second != 0);
            first = first / second;
        }
    }
}

//NAME: tokenizeExpression
//DESCRIPTION:  The top level function which begins the parse.  This function is called 
//              also when parsing from parentheses.  Despite being the top level function,
//              this function is actually the last one to do any evaluation, which is why it
//              handles addition and subtraction.
//INPUT/OUTPUT:
//    eq  - The pointer to where we currently are in the expression.
//    countParenthesis - The counter which keeps track of how many parentheses we've come across.
//                       Should be back to zero when the expression is done.
//RETURNS:
//    The float value calculated from the expression.
float tokenizeExpression(const char*& eq, int& countParenthesis)
{
    //Always scan for a multiplication or division first, as these have higher priority.
    //We might end up back in this function through this call.
    double first = tokenizeMulDiv(eq, countParenthesis);
    while (true)
    {
        //Ignore spaces.
        while (*eq == ' ')
            eq++;

        //Similar to tokenizeMulDiv, except this time with addition or subtraction.
        //This is also what boots us out of the recursion when everything is done.
        char opr = *eq;
        if (opr != '+' && opr != '-')
            return first;
        eq++;

        //If we've gotten this far, it means that we legitimately have an addition or
        //subtraction; however the second number might be an expression itself.
        //Let's check for this.
        double second = tokenizeMulDiv(eq, countParenthesis);
        if (opr == '+')
            first = first + second;
        else
            first = first - second;
    }
}

static float solve(const char* eq)
{
    assert(eq != NULL);

    int pCount = 0;
    float answer = tokenizeExpression(eq, pCount);

    //We shouldn't have unmatched parentheses unless the expression was malformed.
    assert(pCount == 0); 

    return answer;
}

const char* const kExpressions[] = {
    "-((6+4))* -(2+2) - -1",
    "6/5-4-45+3.08",
    "0.34+ -34/45-2",
    "(0.03)*73-2",
    "(20-23 + -5 * (12 / (34 + 3) - 3))",
    "-25 + 4 * -(32 - 45 / 5 - -6)",
    "0.0003101 - 34 * (4 + 5) / 23",
    "1 + ((1 + 1) + 3) + 4 * 5 / 6 - 7",
    "9 / 8/7 /6/5/4  /  3 /  2/1",
    "-( -(-( -(2+3*4)+2 )-1)+ 0)",
};

//These are rounded.
const float kAnswers[] = {
    41.0f,
    -44.72f,
    -2.41556f,
    0.19f,
    10.3784f,
    -141.0f,
    -13.3040f,
    2.33333f,
    0.00022321f,
    11.0f,
};

int main(int argc, char* argv[])
{
    for(int i = 0; i < (sizeof(kExpressions) / sizeof(kExpressions[0])); ++i)
        printf("Expression #%d: %s = %g = %g\n", i, kExpressions[i], solve(kExpressions[i]), kAnswers[i]);
    
    return 0;
}
