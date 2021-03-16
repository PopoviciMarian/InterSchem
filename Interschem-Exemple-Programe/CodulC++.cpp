#include<iostream>
#include<cmath>

using namespace std;
int main(){
    double A, B, C, D, X1, X2, X; 
    cin>>A;
    cin>>B;
    cin>>C;
    D=pow(B,2)-4*A*C;
    if(D>0){
        X1=(-B+sqrt(D))/2*A;
        X2=(-B-sqrt(D))/2*A;
        cout<<X1<<"\n";
        cout<<X2<<"\n";
        return 0;
    }
    else{
        if(D==0){
            X=(-B+sqrt(D))/2*A;
            cout<<X<<"\n";
            return 0;
        }
        else{
            cout<<"Imposbil"<<"\n";
            return 0;
        }
    }
}
