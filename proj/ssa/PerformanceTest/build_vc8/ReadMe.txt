    
    テスト１
    
    vector<int> vec(LOOPS);
    int sum = 0;
    vector<int>::iterator end = vec.end();
    time.start();
    for (vector<int>::iterator i = vec.begin(); i != end; ++i) {
      sum += *i;
      tick(sum);
    }
    time.stop();
    
    テスト２
    
    vector<int> vec(LOOPS);
    int sum = 0;
    struct Sum {
      int& sum_;
      Sum(int& sum) : sum_(sum) {}
      void operator()(int val) {
        sum_ += val;
        tick(sum_);
      }
    };
    time.start();
    for_each(vec.begin(), vec.end(), Sum(sum));
    time.stop();   
    
    テスト３
    
    int* vec = new int[LOOPS];
    int* v = vec;
    int sum = 0;
    time.start();
    for (int n = 0; n < LOOPS; ++n, ++v) {
      sum += *v;
      tick(sum);
    }
    time.stop();