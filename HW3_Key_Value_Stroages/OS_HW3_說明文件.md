# OS_HW3_說明文件
### 學號:F74072227
### 姓名:許湘苡
### 系級:資訊111

---
## 開發環境 :
* 	OS: Ubuntu 20.04.1
*	CPU: Intel(R) Core(TM) i5-8250U CPU @ 1.60GHz
*	Memory: 12GB
*	Programming Language(version): C++11
*	CPU:四核心

---
## 程式執行時間 : 
* page大小 = 1024，測example測資

| key size | 1   | 5   | 10  | 20  | 50  | 128 |
| -------- | --- | --- | --- | --- | --- | --- |
| Time     | 0.045 sec    |  0.024 sec   |  0.024 sec   |  0.027 sec   |  0.026 sec   |  0.024 sec   |

* key大小 = 128，測example測資

| page size | 1            | 10  | 50  | 100 | 500 | 1024 |
| --------- | ------------ | --- | --- | --- | --- | ---- |
| Time      | 0.061 sec | 0.058 sec    |   0.042 sec  |  0.026 sec   | 0.025 sec    |   0.028 sec   |

由上述測量時間發現，page跟key的size大小均與執行時間成反比，page跟key的size越大，執行時間越短。因為example測資沒有很大，所以在上述當page跟key開很大，表現差異較不明顯。

---
## 程式開發與使用說明 :
### 程式開發
* 存取資料位置分成memory和disk兩個部分，使用page table的方式存取資料，以達成檔案有效率的讀取寫入
* 定義每個page可存128個連續編號的key和其相對應的value，memory中有1024個page，其餘的key則以檔案方式存在disk中，每次資料更新均以一個page為單位，一次將128個key從memory移到disk，或從disk移到memory，以減少資料搬移次數，提高效率。
* 以雙層struct定義變數，struct page中再包含struct key，page的編號定義為page[i].index = (key/128)，二進位表示為key的前7個bit。page中128個key的編號定義為value[i] = (key%128)。因為page中的128個value為連續的key所對應到的值，所以若還沒有被PUT到的，則存成-1。
* 當memory中的page都full時，再GET到的值會以.tmp檔案的方式存到disk中的storage資料夾，檔名為(key/128).tmp，每個檔案中有128個key和其對應的value。
* 下圖為memory中data存取的示意圖:

![](https://i.imgur.com/37fJgjz.png)
* 程式邏輯
    1. 讀取input檔案中的指令，一次讀取一行，並依據讀取的結果，分別做PUT、GET、SCAN三個指令的處理，設定一個變數存取memory中的page在GET與SCAN時hit的次數。
    2. 指令若為PUT，先判斷此key對應到的page是否在memory中 :
        (1) page在memory中 : 從此page尋找該key，並將此key的value更新。
        (2) page不在memory中 : 判斷memory中的page數量是否full :
        - 若memory中的page還沒full : add此key對應到的page到memory中，並更新該key的value。
        - 若memory中的page已經full : 到disk中尋找此key對應到的page的tmp檔，並將value更新。
    3. 指令若為GET，先判斷此key對應到的page是否在memory中 : 
        (1) page在memory中(**key hit page**) : 從此page尋找該key，並將此key的值輸出至ouput檔中。
        (2) page不在memory中(**key doesn't hit page**) : 判斷memory中的page數量是否full : 
        - 若memory中的page還沒full : 將disk中該key所對應到的tmp檔(內含128個key，也就是一個page)，從disk搬到memory中，在memory中新增一個page，移動完成後，再將其要GET的value輸出至output檔中。
        - 若memory中的page已經full : 找尋memory中被hit次數最少的page(也就是該page中的key被GET或SCAN的次數最少)，將此page移到disk中，再將disk中要GET的key所對應到的tmp檔(內含128個key，也就是一個page)，從disk搬到memory中的此page，移動完成後，再將其要GET的value輸出至output檔中。
    4. 指令若為SCAN，先計算出SCAN的key1~key2所對應到的page，將對應到相同page的key，統一做一次page搬移的判斷(也就是前一點提到的部分)，搬移完成後，再將其對應到的value一個一個輸出至output檔中。
    5. 待所有指令到執行完畢後，須將memory中的所有page，存進disk中，以供下一次執行此程式時可存取此次的資料。

###	使用說明
*	hw3.cpp
    
    ``` bash=
    # Compile:
    $ g++ hw3.cpp -o hw3 -std=c++11 -lstdc++fs
    # run: 
	# data path為相對路徑，ex:./1.input
    $ ./hw３ [data path]
	```
:::warning
**==第一次==執行hw3.cpp程式後會產生名稱為storage的資料夾，storage內會有多個暫存檔，以提供第二、三個的input檔案中的指令，可以取得第一個input檔案新增或是更新的資料。
若測試中需要重新從第一個input檔開始執行測試，==需將storage資料夾刪除==，再重新執行hw3.cpp**
:::

---
## 分析報告 : 
###  1. 整理資料作法
* 由於input檔案大於RAM的大小，資料的存取沒辦法全部在memory中完成，需要將資料存在memory及disk中，又考量到讀寫disk與讀寫memory的速度落差甚大，所以採用==page table==的方式，將memory中的資料以page的方式分類，每次在disk與memory間==搬移資料均以page為單位==，大幅減少讀寫disk的次數，以提高效率。
* 下圖為memory中資料存取方式的示意圖 : 
![](https://i.imgur.com/37fJgjz.png)
* 由上圖所示，本程式將memory切成1024個page區塊，每個page中包含==連續==的128個key以及其所對應到的value，所有的資料讀寫均以page的角度來執行，每個key為0~2^63^-1，將key/128所得到的值為page的編號，程式中以page[i].index表示，也就是key的前7個bit為此key的page編號，而key%128所得到的值為圖中value的編號。至於disk中的tmp檔檔名的命名以及檔案內容的存取，與memory中的page相同，tmp檔檔名相當於page[i].index，內容為page所存取的128個value。
    * 舉例來說，++若第一個PUT的key = 128001++，則他會被放到上圖中page[0]的位置，此時page[0].index = 128001/128 = 1000，又128001%128 = 1，可以得知，此key及其對應到的value會被++放到上圖的page[0]的value[1]格子中++，並記錄page[0].index = 1000，表示第0個page的編號為1000。由上述可知，第0個page中所存的key為key = 128000~128127。
    * ++若第二個PUT的key = 2560++，因2560/128 = 20，且2560%128 = 0，所以此key會被++放到上圖page[1]的value[0]格子中++，並記錄page[1].index = 20，表示第一個page的編號是20。由上述可知，第1個page中所存的key為key = 2560~2687。
    * ++若第三個PUT的key = 128005++，因128005/128 = 1000，且128005%128 = 5，所以此key會被++放到++上圖中index = 1000的page，跟上述第一個PUT的page相同，也就是++page[0]的value[5]中++。
    * 以此類推，直到memory中的page被放滿為止。
* 當接收到PUT指令時，會先判斷該key對應到的page是否存在在memory中，若存在memory中可以直接更新，若不存在memory中，先將memory中最少使用到的(最少被GET或SCAN的)page swap到disk中，再到disk中尋找該page的tmp檔，將其移動到memory後，再output。
* 因為有SCAN的指令，需要調用到==連續==key所對應到的value，於是規畫出page的內容為連續的128個key及value，使得在執行SCAN指令時，若發現要SCAN的key所對應到的page在disk中，只需要在第一個key時將此page從disk搬移到memory中一次就好，剩餘要SCAN的key就能直接從memory中的page讀取資料。
* 在所有指令執行完畢後，須將memory中的page存到disk中，使資料可以暫存，以供下次讀取使用。
### 2. 存取效率
* page的設計概念 : 此程式中page會在每次GET時，檢查是否需要將page在memory與disk搬移，以達到在memory中的page永遠是最常且最近被GET或SCAN的資料，這樣的設計在要時常讀取相同page的資料時，可以確保其不需要一直到disk抓取資料，只需要調用memory中的資料即可，對於處理大於RAM的input，可以提高其效率。
* key的設計概念 : 主要影響SCAN的效率，因為SCAN是GET連續的key，若沒有使用page概念，則可能反覆讀寫disk，造成執行時間拉長。以本程式的設計，一個page可存取128個key及value，換句話說，++有使用page概念的程式++和沒有使用page概念的程式，光是++SCAN一次，最少就可以減少127次資料在disk跟memory中搬移的次數++，對於要處理大於RAM的資料而言，可以大幅提升其效率。
### 3. 對系統資源分配的觀察
* 此次的程式對系統分配資源差異主要表現在memory與CPU的使用率。
    * 對於沒有使用page的程式，其memory的使用率相較於CPU的使用率高，因為每個指令均須從disk讀寫。
    * 對於有使用page的程式，其CPU的使用率相較於memory的使用率高，因為程式主要執行運算在CPU。
* 在PUT時，CPU使用率在5%~40%浮動，memory使用率維持在0.1%，推測因為在PUT時每次只會從input檔案中抓取一行指令，並運算，故memory使用率較低。
![](https://i.imgur.com/8AM5KNr.png)
* 在GET跟SCAN時，CPU使用率在35%~45%浮動，memory使用率維持在0.1%，CPU使用率較PUT時高很多，因為在此時須做大量運算。
![](https://i.imgur.com/VFXzHUb.png)


### 4. 改進程式的過程
* 一開始在設計page的存取內容時，每個page的內容不是連續的key，而是依據key被PUT的順序來存取，原本的想法是，這樣的存取方式可以確保memory中的page永遠是最近被PUT的資料，但這樣的設計會造成以下兩個問題:
    1. 在GET時，每一次的GET指令都要將所有page檢查過一次，才能知道GET的key是否在memory中，若不在memory中，要到disk中找尋時，則要將storage資料夾中所有的tmp檔案搜尋過一次，會大大降低效率，發現此種作法會使系統在GET時，因為沒有任何規則，所以要做地毯式搜尋，會很耗時。
    2. 在SCAN時，因為要GET大量連續資料，而此種做法所存取的資料亂無章法，會導致系統反覆讀寫disk，造成效率大大降低。
* 改良後第二個版本，每個page的內容為一個範圍的key，但這些key沒有按照順序排列。此種作法雖然比第一種方法有效率且有規則許多，但在SCAN時，每SCAN一個key，便需要將該page中所有的key地毯式搜索，找到該key再output，觀察後發現還可再改進，於是衍伸出第三個版本，也就是現在最新的版本。
* 最新的版本，每個page中的key是按照順序排列的，若遇到還沒有value的key，則將此value存成-1，這樣的做法在SCAN時，可以直接到SCAN的key所對應的page中的該格去抓取值，會再提高效率。





