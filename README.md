# MCF8316A_I2C Arduino Library

MCF8316Aセンサレス・フィールド指向制御（FOC）統合FET BLDCドライバー用のArduinoライブラリです。

このライブラリは、Texas Instruments社のMCF8316A BLDCモーターコントローラーをI2C通信経由で制御するための包括的な機能を提供します。

## 特徴

- **I2C通信による完全制御**: 32ビット/16ビットレジスタの読み書き
- **モーター制御**: 起動、停止、方向制御、速度設定
- **高度な機能**: スピードプロファイル、トルク制御、ブレーキング
- **状態監視**: フォルト検出、アルゴリズム状態、リアルタイム速度フィードバック
- **EEPROM管理**: 設定の保存/読み込み
- **デバッグ機能**: I2C通信のデバッグ出力、CRC-8エラーチェック

## 対応デバイス

- MCF8316A (Texas Instruments)
- 動作電圧: 12-24V (最大40V)
- 最大電流: 8A ピーク
- BLDCモーターまたはPMSMモーター対応

## インストール

1. このリポジトリをArduinoのlibrariesフォルダにダウンロード
2. Arduino IDEを再起動
3. スケッチ → ライブラリをインクルード → MCF8316A_I2C を選択

## 配線

```
MCF8316A    Arduino
--------    -------
SDA    <->  SDA
SCL    <->  SCL
GND    <->  GND
```

**注意**: SDAとSCLラインには4.7kΩのプルアップ抵抗が必要です。

## 基本的な使い方

```cpp
#include <MCF8316A_I2C.h>

MCF8316A_I2C motor;

void setup() {
  // I2C通信を初期化
  if (!motor.begin()) {
    // 初期化失敗
    while(1);
  }
  
  // モーターパラメータを設定
  motor.setMotorParameters(100, 100); // 100mΩ, 100μH
  
  // スピード制御を有効化
  motor.enableSpeedLoop();
  
  // モーターを起動
  motor.setSpeed(1000); // 1000 RPM
  motor.start();
}

void loop() {
  // 現在の速度を取得
  float speed = motor.getSpeed();
  
  // フォルトをチェック
  if (motor.isFault()) {
    motor.clearFaults();
  }
}
```

## API リファレンス

### 初期化

- `begin(address, wire)` - デバイスを初期化
- `setI2CAddress(address)` - I2Cアドレスを変更

### モーター制御

- `start()` - モーターを起動
- `stop()` - モーターを停止
- `coast()` - 惰性停止
- `brake()` - ブレーキをかける
- `setSpeed(rpm)` - 速度を設定（RPM）
- `getSpeed()` - 現在の速度を取得
- `setDirection(forward)` - 回転方向を設定
- `getDirection()` - 現在の回転方向を取得

### ステータスとフォルト

- `getFaultStatus()` - フォルト状態を取得
- `getAlgorithmState()` - アルゴリズム状態を取得
- `clearFaults()` - フォルトをクリア
- `isFault()` - フォルトの有無を確認
- `isRunning()` - モーターが動作中か確認

### 設定管理

- `saveToEEPROM()` - 設定をEEPROMに保存
- `loadFromEEPROM()` - EEPROMから設定を読み込み
- `setMotorParameters(R, L)` - モーターパラメータを設定

### レジスタアクセス

- `readRegister32(address)` - 32ビットレジスタを読み取り
- `writeRegister32(address, data)` - 32ビットレジスタに書き込み
- `readRegister16(address)` - 16ビットレジスタを読み取り
- `writeRegister16(address, data)` - 16ビットレジスタに書き込み

## サンプルスケッチ

- **basic_control** - 基本的なモーター制御
- **speed_control** - 高度な速度制御とシリアルコマンド
- **read_status** - ステータスレジスタの読み取りとモニタリング

## トラブルシューティング

### 初期化に失敗する場合

1. I2C配線を確認（SDA、SCL、GND）
2. プルアップ抵抗（4.7kΩ）が接続されているか確認
3. 電源電圧が適切か確認（12-24V）
4. I2Cアドレスが正しいか確認（デフォルト: 0x01）

### モーターが回転しない場合

1. モーター配線を確認（OUTA、OUTB、OUTC）
2. フォルト状態を確認 `getFaultStatus()`
3. アルゴリズム状態を確認 `getAlgorithmState()`
4. モーターパラメータが正しく設定されているか確認

### I2C通信が不安定な場合

1. データシートに記載されているように、モーター回転中のI2C通信は不安定になる可能性があります
2. 設定はモーター停止中に行うことを推奨
3. CRCを有効にしてエラーチェックを行う `enableCRC()`

## 注意事項

- このライブラリはI2C制御に特化しています。PWM制御には対応していません
- モーター回転中のI2C通信は不安定になる可能性があります（MCF8316C-Q1で改善予定）
- 各バイト転送間に100μsの遅延が必要です（ライブラリで自動処理）

## ライセンス

MIT License

## 貢献

プルリクエストを歓迎します。大きな変更の場合は、まずissueを開いて変更内容について議論してください。
