# pageReplaceOS
2021_OS页面置换,采用ARC算法，通过C++实现，并进行了优化。  
采用了ARC算法(对于参加页面竞速的同学 强烈推荐此算法)
  
利用linux系统下指令perf进行评测,最终rank:  
指令数rank:1/250

* Simulated the page replacement of cache in operating system, and used Adaptive Replacement Cache (ARC) algorithm to decrease miss rate, which considered both recency and frequency, with adaptivity to the current memory access mode by adjusting capacity of cache for both dynamically
* Built data structure with specific function and low memory usage from scratch rather than using STL container
* Ranked 1/250 (according to the weighted result of perf instruction on Linux operating system)

