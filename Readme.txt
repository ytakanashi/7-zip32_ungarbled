_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
【 ソフト名 】　7-zip32.dll/7-zip64.dll/7z.dll文字化け対策版
【バージョン】　9.38.00.01 beta
【 製作者名 】　x@rgs
【 動作環境 】　Windows XP以降
【 製作言語 】　C++
【ｿﾌﾄｳｪｱ種別】　フリーソフトウェア
【 配布条件 】　GNU Lesser General Public License (LGPL)
【  連絡先  】	Y.R.Takanashi@gmail.com
【  サイト  】	http://www16.atpages.jp/rayna/index.html
_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

●説明
 本ライブラリは、文字化け対策を行った「7-zip32.dll」「7-zip64.dll」「7z.dll」です。
 秋田 稔氏作のライブラリ「7-zip32.dll」を64bit化対応させた市丸 剛氏の私家版に、
 Yak!氏による対 zip UTF-8 強制試行版 7z.dll (UTF-8 MAC 対応)パッチの内容を取り込み、
 Info-ZIP Unicode Path Extra Field(0x7075)を読み込むように修正したものです。

 このバージョンは7-Zip 9.38 betaと7-zip32.dll Ver.9.22.00.01をベースにしています。
 辛うじてビルド出来るように修正したのみで、何が起こるかわかりません。
 不安な方は、オリジナルのVer.9.38.xx.xxが公開されるまでお待ちくださいませ。



●ファイル構成
 7z.dll                  ----- 32bit版ライブラリ本体ファイル(本家)
 7-zip32.dll             ----- 32bit版ライブラリ本体ファイル(統合アーカイバ仕様)
 copying.txt             ----- LGPLライセンスファイル
 Readme.txt              ----- このファイル
 Src.7z                  ----- 7-zip32.dll/7-zip64.dll/7z.dllソースファイル(差分)
 ./x64
    7z.dll               ----- 64bit版ライブラリ本体ファイル(本家)
    7-zip64.dll          ----- 64bit版ライブラリ本体ファイル(統合アーカイバ仕様)



●インストール
 1.「7z938001b_ungarbled.zip」を適当なディレクトリに解凍して下さい。
 2.「7-zip32.dll」「7-zip64.dll」をシステムディレクトリにコピーして下さい。
   「7z.dll」は7-zipをインストールしたディレクトリ(例: C:\Program Files\7-Zip)にコピーして下さい。

 ライブラリの動作には「Normaliz.dll」が必要です。ほとんどの環境ではインストールされています。
 もし存在せずエラーが出るようでしたら、
 Yak!氏(http://yak3.myhome.cx:8080/junks/)のサイトにて再配布されていますので、ダウンロードして下さい。



●アンインストール
 レジストリは一切使用しないため、ライブラリ本体ファイルを削除するだけです。



●ビルド方法(開発者向け)
 ・7-Zipは9.38 beta、7-zip32.dllはVer.9.22.00.01、
   コンパイラはMicrosoft Visual C++ 2010 Expressを想定しています。
 ・64bit版をビルドするにはWindows7.1SDKが必要です。
 1.「7z938-src.7z」(http://sourceforge.net/projects/sevenzip/files/)をダウンロード、
   「7z938」ディレクトリを作成し、その中に解凍します。
 2.「7-zip32.dll」「7-zip64.dll」であれば、
    1.「7z938/CPP/7zip/Bundles」に「7-zip32」ディレクトリを作成します。
    2.「7z920001.zip」(http://akky.xrea.jp/)をダウンロード、解凍します。
    3.「src.7z」を解凍し、作成した「7-zip32」ディレクトリにファイルをコピーします。
 3.本ライブラリの「Src.7z」を解凍し、ファイルを「7z938」に上書きします。
 4.「7-zip32.dll」「7-zip64.dll」は
     「7z938/CPP/7zip/Bundles/7-zip32/7-zip32.sln」
   「7z.dll」は
     「7z938/CPP/7zip/Bundles/Format7zF/Format7z.sln」
   を開きます。
 5.「ビルド」->「ソリューションのビルド」でビルドを開始します。



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
 本ライブラリについて、Igor Pavlov氏、秋田 稔氏、市丸 剛氏、Yak!氏に問い合わせないようお願いします。

 サポート(ご意見、ご感想、不具合報告、要望等)は
 Y.R.Takanashi@gmail.com
 若しくは
 BBS(以下URLよりアクセス可能)にて、

 最新版は
 http://www16.atpages.jp/rayna/index.html
 でお願いします。



●開発履歴
 ○Ver.9.38.00.01 beta - 2015/01/23
 ・7-Zip 9.38 betaをベースにビルド。

 ○Ver.9.22.00.01+ - 2015/01/21
 ・Local file headerとCentral directory headerでフラグが異なる書庫の解凍が出来るように。
 ・7z.dllを同梱するように。

 ○Ver.9.22.00.01 - 2015/01/02
 ・7-zip32.dll/7-zip64.dll Ver.9.22.00.01をベースに作成。


_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
This Readme file made by x@rgs
