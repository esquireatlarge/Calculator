# Calculator
A calculator in C that will calculate an infix expression.  Does not use Shunting Yard.

The style used by this program is called a "Recursive Descent Parser."  There are several examples that can be found across the internet.
One of the annoyances when dealing with algebraic expressions as we understand them is the parenthesis.  For humans this is no problem, they help ensure order of operations and make the expression easier to read; for computers this is more of a hassle.

A recursive descent parser avoids the Shunting Yard's algorithm use of data structures (typically stacks and queues) for some memory efficiency.  Each algorithm has its pros and cons, and Shunting Yard is a more commonly used algorithm.
