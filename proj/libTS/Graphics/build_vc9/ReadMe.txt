Windows VisualStudio 2005でのビルド方法

1) proj/lib/boost_1_33_1.zip を同じフォルダに展開する
2) VSのオプション-プロジェクト-インクルードフォルダに、展開したboost_1_33_1フォルダを加える
3) 同じく、proj/lib/glutをインクルードフォルダに加える
4) 同じく、ライブラリフォルダにproj/lib/win32を加える

以上。

注意

現時点では、boostのビルドは不要だが、将来的には必須になる。
boost-jam-3.1.13-1-ntx86.zip を展開し、boostのビルドを行うこと。
