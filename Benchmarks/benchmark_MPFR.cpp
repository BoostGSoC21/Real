 #include<benchmark/benchmark.h>
 #include<bits/stdc++.h>
 #include "include/helper.hpp"
 #include<gmp.h>
 #include<mpfr.h>
 
 #define M 100000
 #define precision 40
 
 //MPFR uses precision in bits, 3 bits (approx) are used for one digit in base 10.
 //Hence bit_precision is calculated.
 #define bit_precision precision*3 
static void BM_add_real(benchmark:: State& state){
     int n=state.range(0);
     char* ar_a=new char[n+2];
     char* ar_b=new char[n+2];
     
	 mpfr_t a,b,r;
	 mpfr_init2(a,bit_precision);
	 mpfr_init2(b,bit_precision);
	 mpfr_init2(r,bit_precision);
	 
	 for(auto _:state){
		 
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 std::string str_b=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
		  
		 mpfr_set_str(a,ar_a,10,MPFR_RNDN);
		 mpfr_set_str(b,ar_b,10,MPFR_RNDN);
		 
		 
		 state.ResumeTiming();

		 mpfr_add(r,a,b,MPFR_RNDN);
	 }
	 
	 mpfr_clear(a);
	 mpfr_clear(b);
	 mpfr_clear(r);
	 delete(ar_a);
	 delete(ar_b);
	 
}

static void BM_multiply_real(benchmark:: State& state){
     int n=state.range(0);
     char* ar_a=new char[n+2];
     char* ar_b=new char[n+2];
     
	 mpfr_t a,b,r;
	 mpfr_init2(a,bit_precision);
	 mpfr_init2(b,bit_precision);
	 mpfr_init2(r,bit_precision);
	 
	 for(auto _:state){
		 
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 std::string str_b=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
		  
		 mpfr_set_str(a,ar_a,10,MPFR_RNDN);
		 mpfr_set_str(b,ar_b,10,MPFR_RNDN);
		 
		 
		 state.ResumeTiming();

		 mpfr_mul(r,a,b,MPFR_RNDN);
	 }
	 
	 mpfr_clear(a);
	 mpfr_clear(b);
	 mpfr_clear(r);
	 delete(ar_a);
	 delete(ar_b);
}

static void BM_divide_real(benchmark:: State& state){
     int n=state.range(0);
     char* ar_a=new char[n+2];
     char* ar_b=new char[n+2];
     
	 mpfr_t a,b,r;
	 mpfr_init2(a,bit_precision);
	 mpfr_init2(b,bit_precision);
	 mpfr_init2(r,bit_precision);
	 
	 for(auto _:state){
		 
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 std::string str_b=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
		  
		 mpfr_set_str(a,ar_a,10,MPFR_RNDN);
		 mpfr_set_str(b,ar_b,10,MPFR_RNDN);
		 
		 
		 state.ResumeTiming();

		 mpfr_div(r,a,b,MPFR_RNDN);
	 }
	 
	 mpfr_clear(a);
	 mpfr_clear(b);
	 mpfr_clear(r);
	 delete(ar_a);
	 delete(ar_b);
}
static void BM_power(benchmark::State& state){
     int n=state.range(0);
     char* ar_a=new char[n+2];
     char* ar_b=new char[n+2];
     
	 mpfr_t a,b,r;
	 mpfr_init2(a,bit_precision);
	 mpfr_init2(b,bit_precision);
	 mpfr_init2(r,bit_precision);
	 
	 for(auto _:state){
		 
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 std::string str_b=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		 toCharArray(str_b,ar_b);
		  
		 mpfr_set_str(a,ar_a,10,MPFR_RNDN);
		 mpfr_set_str(b,ar_b,10,MPFR_RNDN);
		 
		 
		 state.ResumeTiming();

		 mpfr_pow(r,a,b,MPFR_RNDN);
	 }
	 
	 mpfr_clear(a);
	 mpfr_clear(b);
	 mpfr_clear(r);
	 delete(ar_a);
	 delete(ar_b);
}

static void BM_sqrt(benchmark::State& state){
     int n=state.range(0);
     char* ar_a=new char[n+2];
     
	 mpfr_t a,r;
	 mpfr_init2(a,bit_precision);
	 mpfr_init2(r,bit_precision);
	 
	 for(auto _:state){
		 
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		  
		 mpfr_set_str(a,ar_a,10,MPFR_RNDN);
		 
		 state.ResumeTiming();

		 mpfr_sqrt(r,a,MPFR_RNDN);
	 }
	 
	 mpfr_clear(a);
	 mpfr_clear(r);
	 
	 delete(ar_a);
}

static void BM_exp(benchmark::State& state){
     int n=state.range(0);
     char* ar_a=new char[n+2];
     
	 mpfr_t a,r;
	 mpfr_init2(a,bit_precision);
	 mpfr_init2(r,bit_precision);
	 
	 for(auto _:state){
		 
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		  
		 mpfr_set_str(a,ar_a,10,MPFR_RNDN);
		 
		 state.ResumeTiming();

		 mpfr_exp(r,a,MPFR_RNDN);
	 }
	 
	 mpfr_clear(a);
	 mpfr_clear(r);
	 
	 delete(ar_a);
}

static void BM_log(benchmark::State& state){
     int n=state.range(0);
     char* ar_a=new char[n+2];
     
	 mpfr_t a,r;
	 mpfr_init2(a,bit_precision);
	 mpfr_init2(r,bit_precision);
	 
	 for(auto _:state){
		 
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		  
		 mpfr_set_str(a,ar_a,10,MPFR_RNDN);
		 
		 state.ResumeTiming();

		 mpfr_log(r,a,MPFR_RNDN);
	 }
	 
	 mpfr_clear(a);
	 mpfr_clear(r);
	 
	 delete(ar_a);
}

static void BM_sin(benchmark::State& state){
     int n=state.range(0);
     char* ar_a=new char[n+2];
     
	 mpfr_t a,r;
	 mpfr_init2(a,bit_precision);
	 mpfr_init2(r,bit_precision);
	 
	 for(auto _:state){
		 
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		  
		 mpfr_set_str(a,ar_a,10,MPFR_RNDN);
		 
		 state.ResumeTiming();

		 mpfr_sin(r,a,MPFR_RNDN);
	 }
	 
	 mpfr_clear(a);
	 mpfr_clear(r);
	 
	 delete(ar_a);
}

static void BM_cos(benchmark::State& state){
     int n=state.range(0);
     char* ar_a=new char[n+2];
     
	 mpfr_t a,r;
	 mpfr_init2(a,bit_precision);
	 mpfr_init2(r,bit_precision);
	 
	 for(auto _:state){
		 
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		  
		 mpfr_set_str(a,ar_a,10,MPFR_RNDN);
		 
		 state.ResumeTiming();

		 mpfr_cos(r,a,MPFR_RNDN);
	 }
	 
	 mpfr_clear(a);
	 mpfr_clear(r);
	 
	 delete(ar_a);
}

static void BM_tan(benchmark::State& state){
     int n=state.range(0);
     char* ar_a=new char[n+2];
     
	 mpfr_t a,r;
	 mpfr_init2(a,bit_precision);
	 mpfr_init2(r,bit_precision);
	 
	 for(auto _:state){
		 
		 state.PauseTiming();
         
		 std::string str_a=randomRealOfLen(state.range(0));
		 
		 toCharArray(str_a,ar_a);
		  
		 mpfr_set_str(a,ar_a,10,MPFR_RNDN);
		 
		 state.ResumeTiming();

		 mpfr_tan(r,a,MPFR_RNDN);
	 }
	 
	 mpfr_clear(a);
	 mpfr_clear(r);
	 
	 delete(ar_a);
}


 BENCHMARK(BM_add_real)->RangeMultiplier(10)->Range(10,M);
 
 BENCHMARK(BM_multiply_real)->RangeMultiplier(10)->Range(10,M/100);
 BENCHMARK(BM_divide_real)->RangeMultiplier(10)->Range(10,M/100);
 
 BENCHMARK(BM_power)->RangeMultiplier(8)->Range(8,1024);
 BENCHMARK(BM_sqrt)->RangeMultiplier(8)->Range(8,1024);
 BENCHMARK(BM_exp)->RangeMultiplier(8)->Range(8,1024);
 
 BENCHMARK(BM_sin)->RangeMultiplier(8)->Range(8,1024);
 BENCHMARK(BM_cos)->RangeMultiplier(8)->Range(8,1024);
 BENCHMARK(BM_tan)->RangeMultiplier(8)->Range(8,1024);
 
 BENCHMARK_MAIN();
