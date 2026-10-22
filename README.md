# マイコン＋ESP32 講座

サークルのマイコン（ESP32）講座の資料です。MkDocs（Material for MkDocs）でサイトにして、GitHub Pages で公開します。

## 手元でプレビューする

[uv](https://docs.astral.sh/uv/) を使います。

```sh
uv run mkdocs serve
```

初回は必要なパッケージが自動で `.venv` に入ります。ブラウザで http://127.0.0.1:8000 を開くと、保存するたびに自動で更新されます。

VSCode 標準の Markdown プレビューでは、囲み（`!!! mame` など）やタブが正しく表示されないので、見た目の確認はこちらで行ってください。

パッケージを追加するときは `uv add <パッケージ名>` を使います（`pyproject.toml` と `uv.lock` が更新されます）。

## 公開

`main` ブランチに push すると、GitHub Actions（`.github/workflows/deploy.yml`）が `gh-pages` ブランチにビルドして公開します。
初回だけ、リポジトリの Settings → Pages で公開元を `gh-pages` ブランチにしてください。

## フォルダ構成

```
docs/               # 資料本体（Markdown）
  index.md          # トップページ
  00-intro/         # 第0部 導入
  01-basics/        # 第1部 基礎
  02-advanced/      # 第2部 応用
  03-making/        # 第3部 製作に向けて
  appendix/         # 付録（ピン配置、部品一覧）
  images/           # 画像（ステップごとにフォルダを分ける）
  stylesheets/      # 豆知識コーナーなどの CSS
code/               # 各ステップのサンプルコード（docs/ から自動生成。直接編集しない）
templates/          # 新しいステップを書くときのテンプレート
tools/              # 補助スクリプト
mkdocs.yml          # サイトの設定・目次
```

## 書くときのルール

- 新しいステップは `templates/step.md` をコピーして書き、`mkdocs.yml` の `nav` に追加する
- 画像は `docs/images/<ステップのフォルダ名>/` に置く
  - スマホの写真は長辺 1200px 程度に縮小してから入れる（リポジトリが重くなるので）
  - 図は SVG で書き出せるならそれがよい
- まだ撮っていない写真・描いていない図は `!!! todo "画像：〇〇"` で場所だけ確保しておく
- 対象は大学生（工学部）なので、です・ます調で、くだけすぎた表現（「〜ってなに？」「〜してみよう」など）は使わない
- 気になりそうな言葉や豆知識は、豆知識コーナー `!!! mame "用語"` で解説する（タイトルの先頭に「☞」が付く）
    - タイトルは必ず `" "` で囲む。囲まないとタイトルにならず、「Mame」と表示される
    - 本筋の説明に必要なことは本文に書き、知らなくても進められることだけを入れる
    - 1ページに1〜3個くらい
- サンプルコードは本文に書き、`code/` には自動で書き出す（下記）
- ピンは [付録：ピン配置](docs/appendix/pins.md) の割り当てに合わせる。新しく使うピンは、ほかのステップと重ならないものを選び、付録に追記する

## サンプルコードの書き出しとビルド確認

本文のコードブロックのうち、`title="src/main.cpp"` や `title="platformio.ini"` が付いたものを、`code/<ページ名>/` に PlatformIO のプロジェクトとして書き出します。

```sh
uv run python tools/extract_code.py
```

- 1ページに複数のプログラムがある場合は、`title="sender/src/main.cpp"` のようにサブフォルダ名を付ける
- `platformio.ini` を書いていないページには、標準の設定（`espressif32 @ 6.9.0`）が使われる
- title の付いていないコードブロック（説明用の断片）は書き出されない

書き出したあとは、PlatformIO でビルドできるか確認してください（`pio run -d code/<ページ名>`）。
