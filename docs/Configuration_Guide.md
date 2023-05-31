# Configuration of Middleware
The configuration of middleware is performed via command-line arguments, which must match the following pattern:
```
[KEY]:[VALUE]
```
## Required Keys:
| Key       | Value                           | Description                                                                                                       |
|---------- |-------------------------------- |------------------------------------------------------------------------------------------------------------------ |
| `ALG_TYPE`| String constanst: `GSA`, `IA` or `MIA` | Type of algorithm used to solve the task.<br />Input the `GSA` to run the Global Search Algorithm, input the `IA` to run the Index Algorithm and input the `MIA` to run the Index Algorithm with Adaptive Constraint Checking Order.|

## Optional Keys:
| Key       | Value                                 | Description                                                                                                  |
|---------- |-------------------------------------- |------------------------------------------------------------------------------------------------------------- |
| `TASK_NUM`| Integer number from 0 to 99.<br />Data type: `int` | Number of the task to be solved.<br />By default will be solved all tasks. |
| `TASK_R`  | Fractional number greater than 1.<br />Data type: `double` | Task parameter: reliability.<br />By default is [DEFAULT_TASK_RELIABILITY](#defined-configuration-constants). |
| `TASK_ACC`| Fractional number greater than 0.<br />Data type: `double` | Task parameter: accuracy.<br />By default is [DEFAULT_TASK_ACCURACY](#defined-configuration-constants). |
| `TASK_EPS`| Fractional number greater than 0.<br />Data type: `double` | Task parameter: delta (epsilon reserved).<br />By default is [DEFAULT_TASK_EPSILON_RESERVED](#defined-configuration-constants). |
| `SCAN_DEN`| Integer number greater than 0.<br />Data type: `int` | Scan parameter: density of the scan construction.<br />By default is [DEFAULT_SCAN_DENSITY](#defined-configuration-constants). |
| `SCAN_KEY`| Integer number greater from 1 to 3.<br />Data type: `int` | Scan parameter: key of the scan construction.<br />By default is [DEFAULT_SCAN_KEY](#defined-configuration-constants). |
| `PRINT_LEVEL`| Integer number from 0 to 2.<br />Data type: `int` | Whether to print points to the file.<br />0 - do not print anything, 1 - print only trial points, 2 - print trial points and functions points.<br />By default is [DEFAULT_PRINT_LEVEL](#defined-configuration-constants). |
| `ITER_LIMIT`| Integer number greater than 1.<br />Data type: `int` | Limit of iterations of the algorithm to solve the task.<br />By default is [ITERATION_LIMIT](#defined-configuration-constants). |
| `CUSTOM_TASK`| File name with specified extension (only `.txt` is supported).<br />Data type: `string` | File name which contains task described according to [pattern](#custom-task-pattern).<br />By default is not defained. If defined then `TASK_NUM` key is ignored. |

## Command Line Input Example
```
global_optimization.exe ALG_TYPE:IA TASK_NUM:5 TASK_EPS:0.001 CUSTOM_TASK:sphere.txt
```

# Configuration of GUI
The configuration of GUI is performed via command-line argument, which is an integer number from 0 to 99. This argument is required.<br />
Before launching a GUI application for a task with a number `N`, it is necessary to configure and run the middleware for solving this task (pass the `N` value by `TASK_NUM` key). <br />Moreover, if the launch is performed for the first time, it is also necessary to pass the `2` value by the `PRINT_LEVEL` key so that the middleware was generated information for plotting functions.<br />
Supported only three-dimensional functions.<br />

## Command Line Input Example
```
python graphics_builder.py 5
```

# Defined Configuration Constants
| Name                            | Value  |
| ------------------------------- |------- |
| `DEFAULT_TASK_RELIABILITY`      | 3      | 
| `DEFAULT_TASK_ACCURACY`         | 0.01   | 
| `DEFAULT_TASK_EPSILON_RESERVED` | 0      |
| `DEFAULT_SCAN_DENSITY`          | 10     | 
| `DEFAULT_SCAN_KEY`              | 1      |
| `DEFAULT_PRINT_LEVEL`           | 1      |
| `ITERATION_LIMIT`               | 5000   |

## Custom Task Pattern
Variable set should consist only lower case Latin letters.<br />
Mathematical operations should consist only upper case Latin letters.<br />
Symbols of mathematical operations (for example multiplication) should not be missed.<br />
```
<variable set> <count of constraints>
<left task border> <right task border>
<function expression>
<constraint 1>
<constraint 2>
...
<optimum point> <optimum value>
```

## Custom Task Example
```
xy 3
0,-1 4,3
-1.5*x^2*EXP(1-x^2-20.25*(x-y)^2)-((0.5*(x-1)*(y-1))^4)*EXP(2-(0.5*(x-1)^4)-(y-1)^4)
0.01*(x-2.2)^2+(y-1.2)^2-2.25
100*(1-((x-2)^2)/1.44-(0.5*y)^2)
10*(y-1.5-1.5*SIN(6.283*(x-1.75)))
0.943359,0.939453 -1.489757
```
