@echo off
REM Quick training script for floating waste detection (Windows)
REM Usage: quick_train.bat [epochs] [device] [batch]

set EPOCHS=%1
if "%EPOCHS%"=="" set EPOCHS=100

set DEVICE=%2
if "%DEVICE%"=="" set DEVICE=cpu

set BATCH=%3
if "%BATCH%"=="" set BATCH=16

echo 🚢 Starting YOLOv8n training for floating waste detection
echo Epochs: %EPOCHS%
echo Device: %DEVICE%
echo Batch size: %BATCH%
echo.

REM Check if dataset YAML exists
if not exist "waste_dataset.yaml" (
    echo ⚠ Dataset YAML not found. Creating template...
    python train_yolo_waste.py --create-yaml
    echo.
    echo Please update waste_dataset.yaml with your class names and dataset paths
    echo Then run this script again.
    exit /b 1
)

REM Run training
python train_yolo_waste.py --data waste_dataset.yaml --epochs %EPOCHS% --device %DEVICE% --batch %BATCH% --imgsz 640

echo.
echo ✅ Training complete!
echo 📊 Check results in: runs\waste_detection\
echo 🎯 Best model: runs\waste_detection\weights\best.pt

