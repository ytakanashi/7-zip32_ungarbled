_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
【 ソフト名 】　7-zip32.dll/7-zip64.dll文字化け対策版
【バージョン】　9.22.00.01
【 製作者名 】　x@rgs
【 動作環境 】　Windows XP以降
【 製作言語 】　C++
【ｿﾌﾄｳｪｱ種別】　フリーソフトウェア
【 配布条件 】　GNU Lesser General Public License (LGPL)
【  連絡先  】	Y.R.Takanashi@gmail.com
【  サイト  】	http://www16.atpages.jp/rayna/index.html
_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

●説明
 本ライブラリは、文字化け対策を行った「7-zip32.dll」「7-zip64.dll」です。
 秋田 稔氏作のライブラリ「7-zip32.dll」を64bit化対応させた市丸 剛氏の私家版に、
 Yak!氏による対 zip UTF-8 強制試行版 7z.dll (UTF-8 MAC 対応)パッチの内容を取り込み、
 Info-ZIP Unicode Path Extra Field(0x7075)を読み込むように修正したものです。



●ファイル構成
 7-zip32.dll             ----- 32bit版ライブラリ本体ファイル
 7-zip64.dll             ----- 64bit版ライブラリ本体ファイル
 copying.txt             ----- LGPLライセンスファイル
 Readme.txt              ----- このファイル
 Src.7z                  ----- 7-zip32.dll/7-zip64.dllソースファイル(差分)



●インストール
 1.「7z922001_ungarbled.zip」を適当なディレクトリに解凍して下さい。
 2.「7-zip32.dll」「7-zip64.dll」をシステムディレクトリにコピーして下さい。

 ライブラリの動作には「Normaliz.dll」が必要です。ほとんどの環境ではインストールされています。
 もし存在せずエラーが出るようでしたら、
 Yak!氏(http://yak3.myhome.cx:8080/junks/)のサイトにて再配布されていますので、ダウンロードして下さい。



●アンインストール
 レジストリは一切使用しないため、ライブラリ本体ファイルを削除するだけです。



●ビルド方法
 ・Microsoft Visual C++ 2010 Express環境を想定しています。
 ・「7-zip32.dll」は下記「開発環境」項目の[Main]と[Sub]で、
   「7-zip64.dll」は[Main]でビルドと動作を確認しています。
 1.「7z922.tar.bz2」(http://sourceforge.net/projects/sevenzip/files/)をダウンロード、
   「7z922」ディレクトリを作成し、その中に解凍します。
 2.「7z922/CPP/7zip/Bundles」に「7-zip32」ディレクトリを作成します。
 3.「7z920001.zip」(http://akky.xrea.jp/)をダウンロード、解凍します。
 4.「src.7z」を解凍し、作成した「7-zip32」ディレクトリにファイルをコピーします。
 5.本ライブラリの「Src.7z」を解凍し、ファイルを「7z922」に上書きします。
 6.「7z922/CPP/7zip/Bundles/7-zip32/7-zip32.sln」を開きます。
 7.
   [7-zip32.dll]
     1.「Release」「Win32」を選択します。
   [7-zip64.dll]
     1.Windows7.1SDKをインストールします。
     2.「Release」「x64」を選択します。
 8.「ビルド」->「ソリューションのビルド」でビルドを開始します。



●開発環境
 [Main]
 OS:Microsoft Windows 7 Home Premium 64-bit (6.1, Build 7600)
 CPU:Intel(R) Core(TM) i5 CPU M 460 @ 2.53GHz (4 CPUs), ~2.5GHz
 memory:4096MB RAM
 compiler/debugger:Microsoft Visual C++ 2010 Express
 editor:xyzzy version 0.2.2.235

 [Sub]
 OS:Microsoft Windows XP Home Edition Build 2600 SP3
 CPU:Intel(R) Atom(TM) CPU N270@1.60GHz,1600MHz(4x400)
 memory:1016MB
 compiler/debugger:Microsoft Visual C++ 2010 Express
 eeditor:xyzzy version 0.2.2.235



●注意
 ・あらゆるzipファイルの文字化けが解消するわけではありません。



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

 また、本ソフトウェアを使用することによって如何なる損害が発生しても
 作者は一切責任を負わないものとします。
 全ては自己責任の元でお使いください。



●参考文献
 APPNOTE.TXT - .ZIP File Format Specification
 http://www.pkware.com/documents/casestudies/APPNOTE.TXT



●その他
 本ライブラリについて、秋田 稔氏、市丸 剛氏、Yak!氏に問い合わせないようお願いします。

 サポート(ご意見、ご感想、不具合報告、要望等)は
 Y.R.Takanashi@gmail.com
 若しくは
 BBS(以下URLよりアクセス可能)にて、

 最新版は
 http://www16.atpages.jp/rayna/index.html
 でお願いします。



●開発履歴
 ○Ver.9.22.00.01 - 2015/01/02
 ・7-zip32.dll/7-zip64.dll Ver.9.22.00.01をベースに作成。


_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
This Readme file made by x@rgs
