#include<stdio.h>

double power(double base , int exponent){
    // if (exponent == 0) {
    //         return 1;
    //     } 
    // else if(exponent > 0) {
    //     return base * power(base, exponent - 1);
    // }
    //     else {
    // // Handling negative exponentonent
    // return 1.0 / (base * power(base, -exponent - 1)); 
    // }  

    double result = 1;
    if(exponent >= 0){
     for(int i = 0; i < exponent; i++){
        result = result * base;
        }
    }
    else{
        for(int i = 0; i > exponent; i--){
            result = result / base;
        }
        // result = 1 / result;
    }

    return result;
}

int main(){

    double a ;
    int b;
    printf("Enter The Number :");
    scanf("%lf", &a);

    printf("Enter The Power :");
    scanf("%d", &b);

    printf("\n");
    printf("%.2lf^%d = %.5lf\n", a, b, power(a, b));
    return 0;
}