_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
【 ソフト名 】　7-zip32.dll/7-zip64.dll/7z.dll文字化け対策版
【バージョン】　21.02.00.01
【 製作者名 】　x@rgs
【 動作環境 】　Windows XP以降
【 製作言語 】　C++
【ｿﾌﾄｳｪｱ種別】　フリーソフトウェア
【 配布条件 】　GNU Lesser General Public License (LGPL)
【  連絡先  】	Y.R.Takanashi@gmail.com
【  サイト  】	http://frostmoon.sakura.ne.jp/
_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

●説明
 本ライブラリは、文字化け対策を行った「7-zip32.dll」「7-zip64.dll」「7z.dll」です。
 秋田 稔氏作のライブラリ「7-zip32.dll」を64bit化対応させた市丸 剛氏の私家版に、
 Yak!氏による対 zip UTF-8 強制試行版 7z.dll (UTF-8 MAC 対応)パッチの内容を取り込み、
 さらにMLangによる文字コード変換機能を追加したものです。
 SevenZipExtractMem()を勝手に実装したりもしました。
 RarやCab書庫等々も処理できる7z.dll対応版「7-zip32.dll」「7-zip64.dll」も同梱しています。

 このバージョンは7-Zip 21.02 alphaと7-zip32.dll Ver.9.22.00.02をベースにした非公式版です。
 辛うじてビルド出来るように修正したのみで、何が起こるかわかりません。
 不安な方は、オリジナルの7-zip32.dll/7-zip64.dllが公開されるまでお待ちくださいませ。

 7z.dll対応版「7-zip32.dll」「7-zip64.dll」は更に何が起こるかわかりません。



●ファイル構成
 7z.dll                  ----- ライブラリ本体ファイル(本家,32bit)
 7-zip32.dll             ----- ライブラリ本体ファイル(統合アーカイバ仕様,32bit)
 copying.txt             ----- LGPLライセンスファイル
 Readme.txt              ----- このファイル
 Src.7z                  ----- 7-zip32.dll/7-zip64.dll/7z.dllソースファイル(差分)
 ./x64
    7z.dll               ----- ライブラリ本体ファイル(本家,64bit)
    7-zip64.dll          ----- ライブラリ本体ファイル(統合アーカイバ仕様,64bit)
 ./7-zip32-full
    7-zip32.dll          ----- 7z.dll対応版ライブラリ本体ファイル(統合アーカイバ仕様,32bit)
    7-zip64.dll          ----- 7z.dll対応版ライブラリ本体ファイル(統合アーカイバ仕様,64bit)



●インストール
 0.インストーラー版は以下1,2の作業は不要です。
 1.「7z2102001_ungarbled.zip」を適当なディレクトリに解凍して下さい。
 2.「7-zip32.dll」「7-zip64.dll」をシステムディレクトリにコピーして下さい。
   「7z.dll」は7-Zipをインストールしたディレクトリ(例: C:\Program Files\7-Zip )にコピーして下さい。
 #.7z.dll対応版「7-zip32.dll」「7-zip64.dll」の動作には7-Zipのインストールが必要です。
   *公式サイト( http://www.7-zip.org/ )で配布されているインストーラを利用することをおすすめします。

 ライブラリの動作には「Normaliz.dll」が必要です。ほとんどの環境ではインストールされています。
 もし存在せずエラーが出るようでしたら、
 Yak!氏( http://yak3.myhome.cx:8080/junks/ )のサイトにて再配布されていますので、ダウンロードして下さい。

 通常版と7z.dll対応版の「7-zip32.dll」「7-zip64.dll」は共存できません。
 ふつうは通常版をインストールして下さい。



●アンインストール
 レジストリは一切使用しないため、ライブラリ本体ファイルを削除するだけです。



●ビルド方法(開発者向け)
 ・7-Zipは21.02 aplha、7-zip32.dllはVer.9.22.00.02、
   コンパイラはMicrosoft Visual C++ 2019を想定しています。
 1.「7z2102-src.7z」( https://www.7-zip.org/ )をダウンロード、
   「7z2102」ディレクトリを作成し、その中に解凍します。
 2.「7-zip32.dll」「7-zip64.dll」をビルドする場合、
    1.「7z2102/CPP/7zip/Bundles」に「7-zip32」ディレクトリを作成します。
    2.「7z922002.zip」( http://akky.xrea.jp/ )をダウンロード、解凍します。
    3.「src.7z」を解凍し、作成した「7-zip32」ディレクトリにファイルをコピーします。
    4.「7-zip32/SFX」ディレクトリにある読み取り専用ファイルを「7z2102/C」からコピーします。
 3.本ライブラリの「Src.7z」を解凍し、ファイルを「7z2102」に上書きします。
 4.「7-zip32.dll」「7-zip64.dll」は
     「7z2102/CPP/7zip/Bundles/7-zip32/7-zip32.sln」
   「7z.dll」は
     「7z2102/CPP/7zip/Bundles/Format7zF/Format7z.sln」
   を開きます。
 5.「ビルド」->「ソリューションのビルド」でビルドを開始します。



●コードページの指定方法
 ・MLangによる文字コード変換を行うには「-mcp」でコードページを指定します。
   例: -mcp=51932
       (EUC-JPを指定)
 ・SevenZip()以外、SevenZipFindFirst()等々も使用する場合、SevenZipSetCP()でコードページを渡して下さい。
   例: SevenZipSetCP(51932)



●7z.dll対応版の「7-zip32.dll」「7-zip64.dll」仕様
 ・動作には7-Zipのインストール(7z.dll)が必要です。
 ・本家7-Zipで対応している形式すべてを扱える...はずです。(以下公式日本語サイト( https://sevenzip.osdn.jp/ )より引用)
      圧縮/解凍(展開): 7z, XZ, BZIP2, GZIP, TAR, ZIP, WIM
      解凍(展開)のみ: AR, ARJ, CAB, CHM, CPIO, CramFS, DMG, EXT, FAT, GPT, HFS, IHEX, ISO, LZH, LZMA, MBR, MSI, NSIS, NTFS, QCOW2, RAR, RPM, SquashFS, UDF, UEFI, VDI, VHD, VMDK, WIM, XAR, Z
 ・7-zip32.dllオリジナルのSFXモジュールは使用できません。デフォルトでは「7zCon.sfx」を使用します。
 ・INDIVIDUALINFO構造体のszModeに圧縮形式は格納されません。(オリジナルで対応しているZipと7zは除く)
 ・SevenZipGetArchiveType()で返される形式は将来変更される可能性があります。(勝手に追加したため)
 ・SevenZipExists7zdll()を実装しています。書庫処理前に呼び出し、「7z.dll」が存在するか確認してください。



●開発環境
 OS:Microsoft Windows 10 Home Premium 64-bit
 CPU:Intel(R) Core(TM) i7-6700 CPU @ 3.40GHz 3.41GHz
 memory:16.0GB RAM
 compiler/debugger:Microsoft Visual Studio Community 2019 Preview Version 16.10.0 Preview 2.1
 editor:xyzzy version 0.2.2.235



●注意
 ・すべてのzipファイルの文字化けが解消するわけではありません。
 ・内蔵SFXは本家と異なりランタイムを静的リンクしています。
 ・このライブラリは非公式版です。
   取り扱いには十分ご注意ください。



●著作権及び転載について
 ファイルの圧縮、解凍の基本部分の著作権はIgor Pavlov氏にあります。
 7z圧縮方式のBZip2アルゴリズムはJulian Seward氏が作成し
 PPMDアルゴリズムはDmitry Shkarin氏が作成しています。
 統合アーカイバ仕様のDLL作成部分に関しては秋田 稔氏が著作権を保有します。
 64bit化対応の改変部分に関しては市丸 剛氏が著作権を保有します。
 UTF-8-MAC対応の改変部分に関してはYak!氏が著作権を保有します。

 本ライブラリはGNU Lesser General Public License (LGPL)
 の元で配布されています。
 ソフトの改変、再配布等についてはLGPLに従ってください。
 http://www.gnu.org/copyleft/lesser.ja.html



●その他
 ・本ライブラリについて、Igor Pavlov氏、秋田 稔氏、市丸 剛氏、Yak!氏に問い合わせないようお願いします。

 ・サポート(ご意見、ご感想、不具合のご報告、ご要望等)は
   Y.R.Takanashi@gmail.com
   若しくは
   BBS(以下URLよりアクセス可能)からお願いします。
   最新版は
   http://frostmoon.sakura.ne.jp/
   から入手することができます。



●開発履歴
 ○Ver.21.02.00.01 - 2021/06/08
 ・7-Zip 21.02 alphaをベースにビルド。
 ・プラットフォームツールセットをv141_xpに変更。

 ○Ver.19.00.00.03 - 2021/01/19
 ・SevenZipFindFirst()で設定したワイルドカードがSevenZipFindNext()で反映されない不具合を修正。(Special Thanks!:HotKeyIt様)

 ○Ver.19.00.00.02 - 2020/08/22
 ・4,294,967,295バイトを超える出力に対応したSevenZipExtractMemEx()を追加。(Special Thanks!:tenteko様)
 ・SevenZipExtractMem()/SevenZipExtractMemEx()でARCEXTRACT_BEGIN(0)及びARCEXTRACT_INPROCESS(1)も送出するように。(Special Thanks!:tenteko様)
 ・SevenZipExtractMem()/SevenZipExtractMemEx()でディレクトリを作成してしまう不具合を修正。(Special Thanks!:tenteko様)
 ・ソリッド書庫等の解凍時、実際に解凍せずスキップするファイルの処理中もプログレスバーの更新や、コールバック関数へメッセージの送信等をするように。
   なお、スキップファイルの処理中はARCEXTRACT_SKIP(5)となります。(Special Thanks!:tenteko様)

 ○Ver.19.00.00.01 - 2019/02/24
 ・7-Zip 19.00をベースにビルド。

 ○Ver.18.06.00.01 - 2018/12/31
 ・7-Zip 18.06をベースにビルド。

 ○Ver.18.05.00.02 - 2018/07/16
 ・上書き確認ダイアログ表示時、更新日時やサイズの取得ができないファイルの場合強制終了する不具合を修正。(Special Thanks!:須藤幸一様)

 ○Ver.18.05.00.01 - 2018/05/06
 ・7-Zip 18.05をベースにビルド。

 ○Ver.18.03.00.01 beta - 2018/03/06
 ・7-Zip 18.03 betaをベースにビルド。
 ・7z.dll対応版でIHex、OBJ/COFFが処理できない不具合を修正。

 ○Ver.18.01.00.01 - 2018/01/30
 ・7-Zip 18.01をベースにビルド。

 ○Ver.18.00.00.01 - 2018/01/13
 ・7-Zip 18.00 betaをベースにビルド。

 ○Ver.17.01.00.01 - 2017/08/29
 ・7-Zip 17.01 betaをベースにビルド。

 ○Ver.17.00.00.01 - 2017/04/30
 ・7-Zip 17.00 beta、7-zip32.dll Ver.9.22.00.02をベースにビルド。

 ○Ver.16.04.00.01 - 2016/10/05
 ・7-Zip 16.04 betaをベースにビルド。

 ○Ver.16.03.00.01 - 2016/09/29
 ・7-Zip 16.03 betaをベースにビルド。

 ○Ver.16.02.00.01 - 2016/05/23
 ・7-Zip 16.02 betaをベースにビルド。

 ○Ver.16.01.00.01 - 2016/05/20
 ・7-Zip 16.01 betaをベースにビルド。
 ・SFXのバージョンを更新し忘れていたのを修正。

 ○Ver.16.00.00.01 - 2016/05/11
 ・7-Zip 16.00 betaをベースにビルド。
 ・自己解凍書庫作成時にタイトルやメッセージを指定すると「Can't load config info」と表示され解凍できない不具合を修正。(Special Thanks!:陸様)
 ・内蔵SFXをUPXで圧縮するように。

 ○Ver.15.14.00.01 - 2016/01/01
 ・7-Zip 15.14 betaをベースにビルド。
 ・公式で対応したため、独自のInfo-ZIP Unicode Path Extra Field(0x7075)に関するコードを削除。

 ○Ver.15.12.00.02 - 2015/11/27
 ・7z.dll対応版でARJ書庫の処理ができない不具合を修正。(Special Thanks!:Roelf Beukens様)
 ・SevenZipGetArchiveType()とSevenZipGetFileCount()について、ヘッダ暗号化書庫であればパスワード入力ダイアログを表示するように。(Special Thanks!:Roelf Beukens様)

 ○Ver.15.12.00.01 - 2015/11/20
 ・7-Zip 15.12をベースにビルド。

 ○Ver.15.11.00.01 beta - 2015/11/19
 ・7-Zip 15.11 betaをベースにビルド。

 ○Ver.15.10.00.01 beta - 2015/11/05
 ・7-Zip 15.10 betaをベースにビルド。

 ○Ver.15.09.00.01 beta - 2015/10/21
 ・7-Zip 15.09 betaをベースにビルド。
 ・SevenZipSetCP(),SevenZipGetCP()を追加し、SevenZipSetUnicodeMode()でのコードページ指定を廃止。
 ・7z.dll対応版「7-zip32.dll」「7-zip64.dll」について、SevenZipExists7zdll()を追加し、通常版と7z.dll対応版の判別、及び7z.dllの存在を確認出来るように。

 ○Ver.15.08.00.02 beta - 2015/10/15
 ・余計なSFXモジュールを誤って内蔵していたのを修正。
 ・7z.dll対応版「7-zip32.dll」「7-zip64.dll」追加。
 ・32bit版ライブラリについてはコンパイラをMicrosoft Visual C++ 2010に変更。

 ○Ver.15.08.00.01 beta - 2015/10/06
 ・7-Zip 15.08 betaをベースにビルド。

 ○Ver.15.07.00.01 beta - 2015/09/24
 ・7-Zip 15.07 betaをベースにビルド。
 ・自己解凍部モジュールをマルチスレッド(/MT)でビルドするように。
 ・作成した自己解凍書庫でプログレスバーが表示されない不具合を修正。
 ・「-mcp」で指定されなければ文字コードの変換を行わないように。

 ○Ver.15.06.00.02 beta - 2015/08/18
 ・SevenZipExtractMem()実装。
 ・コードページ指定をInfo-ZIP Unicode Path Extra Fieldより優先するように。

 ○Ver.15.06.00.01 beta - 2015/08/12
 ・7-Zip 15.06 betaをベースにビルド。
 ・MLangにより文字コードの変換を行うように。
 ・コンパイラをMicrosoft Visual C++ 2015に変更。

 ○Ver.15.05.00.02 beta - 2015/06/29
 ・リスト表示で日本語が文字化けする不具合を修正。(Special Thanks!:kiyohiro様)
 ・ファイル名がUTF-8で格納されている場合でも末尾の'/'からディレクトリ判定を行うように。(Special Thanks!:kiyohiro様)

 ○Ver.15.05.00.01 beta - 2015/06/23
 ・7-Zip 15.05 betaをベースにビルド。

 ○Ver.9.38.00.01 beta - 2015/01/23
 ・7-Zip 9.38 betaをベースにビルド。

 ○Ver.9.22.00.01+ - 2015/01/21
 ・Local file headerとCentral directory headerでフラグが異なる書庫の解凍が出来るように。
 ・7z.dllを同梱するように。

 ○Ver.9.22.00.01 - 2015/01/02
 ・7-zip32.dll/7-zip64.dll Ver.9.22.00.01をベースに作成。


_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
This Readme file made by x@rgs
