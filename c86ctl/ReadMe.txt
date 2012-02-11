---------------------------------------------------------------------
  C86CTL   by honet.kk@gmail.com
---------------------------------------------------------------------

==============
 What's this?
==============
OPNAのパラメータビュアーです。本DLLに対して音源チップへのレジスタ
アクセス情報を送ると、チップの状態を可視化します。

また、ハードウェアＦＭ音源の制御機能を持っており、下記のハードウェア
が接続されている場合は制御を行います。
・G.I.M.I.C.   http://gimic.jp/


============
 使い方
============
未記述


==========
 LICENSE
==========
Copyright (c) 2009-2011, Kiichiro Kotajima. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.


For further information please contact.
 honet.kk (at) gmail.com

============
 ChangeLog
============
1.3.0.9					
1.2.0.8					・CreateInstance関数の呼び出し規約をWINAPIに変更
						　（1.2.0.6で付け忘れました。ごめんなさい。）
						・IRealChipBaseのGUID変更
						　（IRealChipと同じGUIDになっていたバグを修正）
						・HID通信時のトランザクションに同期処理追加
						・IGimic::getMBInfo/getModuleInfo において
						　文字列終端が０にならない場合があるバグを修正

1.2.0.7					・LICENSE文修正（コピーライト文にコピペ消し忘れの変な文が残ってた･･･）
						　※利用条件に変更はありません。
						・OPMのレジスタアクセスフィルタにおいて
						　0x20<addrの時にレジスタキャッシュ値が更新されていなかったバグを修正
						・OPNAのレジスタアクセスフィルタにおいて
						　0x100==addrのアクセスがおかしくなってしまっていたバグを修正

1.2.0.6					HID通信の仕様を大幅変更。gimic-r158以降用
1.1.0.3					バージョンリソース追加

