#include<iostream>
#include<variant>
#include<functional>
#include<utility>

template<typename A, typename B>
using Hom = std::function<B(A)>;

struct Nothing{};

template<typename A>
using Maybe = std::variant<A, Nothing>;

// x -> Tx
template<typename A>
Maybe<A> eta(A x){
    return Maybe<A>(x);
}

// TTx -> x
template<typename A>
Maybe<A> mu(Maybe<Maybe<A>> x){
    if(std::holds_alternative<Nothing>(x)){
        return Nothing();
    }
    return std::get<Maybe<A>>(x);
}

/* Kleisli  */
template<typename A>
Maybe<A> Return(A x){
    return eta(x);
}

template<typename A, typename B>
Maybe<B> Bind(Maybe<A> x, std::function<Maybe<B>(A)> f){
    return mu(Maybef(f)(x));
}


/* 出力用 */
template<typename A>
std::ostream& operator <<(std::ostream& os, const Maybe<A>& Tx){
    if(std::holds_alternative<Nothing>(Tx)){
        os << "Nothing";
    } else {
        os << std::get<A>(Tx);
    }
    return os;
}

/* 割り算 */
Maybe<int> operator/(Maybe<int> a, Maybe<int> b){
    if(std::holds_alternative<Nothing>(a)
        || std::holds_alternative<Nothing>(b)){
        return Nothing();
    }
    int just_a = std::get<int>(a);
    int just_b = std::get<int>(b);
    return just_b ? eta(just_a / just_b) : Nothing();
}

template<typename A, typename B>
Hom<Maybe<A>,Maybe<B>> Maybef(Hom<A,B> f){
    auto res = [=](Maybe<A> x){
        if(std::holds_alternative<Nothing>(x)){
            return Maybe<B>(Nothing());
        }
        return Maybe<B>(f(std::get<A>(x)));
    };
    return res;
}

int main(){
    Maybe<int> a = 10;
    std::cout << a << std::endl; // 10
    std::cout << a / 5 << std::endl; // 2
    std::cout << a/0 << std::endl; // Nothing
    std::cout << a/0/5 << std::endl; // Nothing

	using intxint = std::pair<int,int>;
    Hom<intxint, Maybe<int>> division = [](intxint x){
        auto [a, b] = x;
		return b ? eta(a/b) : Nothing();
	};
	Maybe<intxint> x = intxint({15,0});
    std::cout << Bind(x, division) << std::endl; // Nothing
}
