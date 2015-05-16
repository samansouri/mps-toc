#
#### Algorithms for Master Production Scheduling under the Theory of Constraints

#####			Designed and developed by:
#####   Dr Afshin Mansouri© 
#####	  Brunel Business School, Brunel University, UK
#####	  Email: 
#####		 Afshin.Mansouri@brunel.ac.uk
#####		 samansouri@hotmail.com


####Notes for users: 

Preparation: Make sure you have created two folders named "Input" and "Output" next to your project.
Data sets should be placed in the "Input" folder and the outputs will be saved in the "Output" folder.

Q: How to define a data set?
A: Problem sets should be defined in a text file using the following format and placed in the Input directory:

```
ProdNum=n;
ResNum=m;
Prod_1:CODE,t_11,...,t_1m,D_1,CM_1;
.
.
.
Prod_n:CODE,t_n1,...,t_nm,D_n,CM_m;
Res_1:CODE,C_1,gamma_1;
.
.
.
Res_m:CODE,C_m,gamma_m;
```

Q: How to solve a problem?
A: Put the full name of the data file (including its extension) as an argument in Visual C++ and then run the code. 
Alternatively, you may enter the project name (MPS-TOC) followed by the name of the data set in command line.
 

