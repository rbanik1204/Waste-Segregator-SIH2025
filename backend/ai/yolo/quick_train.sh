#!/bin/bash
# Quick training script for floating waste detection
# Usage: ./quick_train.sh [epochs] [device]

EPOCHS=${1:-100}
DEVICE=${2:-cpu}
BATCH=${3:-16}

echo "🚢 Starting YOLOv8n training for floating waste detection"
echo "Epochs: $EPOCHS"
echo "Device: $DEVICE"
echo "Batch size: $BATCH"
echo ""

# Check if dataset YAML exists
if [ ! -f "waste_dataset.yaml" ]; then
    echo "⚠ Dataset YAML not found. Creating template..."
    python train_yolo_waste.py --create-yaml
    echo ""
    echo "Please update waste_dataset.yaml with your class names and dataset paths"
    echo "Then run this script again."
    exit 1
fi

# Run training
python train_yolo_waste.py \
    --data waste_dataset.yaml \
    --epochs $EPOCHS \
    --device $DEVICE \
    --batch $BATCH \
    --imgsz 640

echo ""
echo "✅ Training complete!"
echo "📊 Check results in: runs/waste_detection/"
echo "🎯 Best model: runs/waste_detection/weights/best.pt"

