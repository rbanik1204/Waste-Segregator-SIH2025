# 🚢 YOLOv8n Training for Floating Waste Detection

Complete training pipeline for detecting waste objects floating on water surfaces.

## 📦 Quick Start

### 1. Install Dependencies

```bash
cd backend/ai
pip install -r docker/requirements.txt
```

### 2. Prepare Dataset

#### Option A: Create Dataset Structure
```bash
python yolo/prepare_dataset.py --source /path/to/images --output data/datasets/waste --split 0.8
```

#### Option B: Manual Setup
```
data/datasets/waste/
├── train/
│   ├── images/  (your training images)
│   └── labels/  (YOLO format .txt files)
└── val/
    ├── images/  (validation images)
    └── labels/  (YOLO format .txt files)
```

### 3. Annotate Images

Use **LabelImg** to annotate:
```bash
pip install labelImg
labelImg
```

- Format: **YOLO** (not PascalVOC)
- Save labels as `.txt` files with same name as images

### 4. Create Dataset YAML

```bash
python yolo/train_yolo_waste.py --create-yaml
```

Edit `waste_dataset.yaml` with your class names.

### 5. Train Model

#### Windows:
```bash
yolo\quick_train.bat 100 0 32
```

#### Linux/Mac:
```bash
chmod +x yolo/quick_train.sh
./yolo/quick_train.sh 100 0 32
```

#### Manual:
```bash
python yolo/train_yolo_waste.py --data yolo/waste_dataset.yaml --epochs 100 --device 0 --batch 32
```

## 🎯 Training Parameters

- **Epochs**: 100-150 (monitor for overfitting)
- **Image Size**: 640px (optimal for floating objects)
- **Batch Size**: 16 (CPU) or 32+ (GPU)
- **Device**: `cpu`, `cuda`, or `0,1,2,3` for GPU

## 📊 Expected Results

After training, check:
- `runs/waste_detection/weights/best.pt` - Best model
- `runs/waste_detection/results.png` - Training curves
- `runs/waste_detection/confusion_matrix.png` - Class performance

**Good metrics:**
- mAP50 > 0.70
- mAP50-95 > 0.50
- Precision > 0.75
- Recall > 0.70

## 🔧 Model Usage

The trained model is automatically copied to:
```
backend/ai/yolo/yolov8n.pt
```

The FastAPI service (`backend/ai/api/main.py`) will use this model automatically.

## 📚 Full Documentation

See `TRAINING_GUIDE.md` for detailed instructions.

## 🆘 Troubleshooting

**Out of Memory:**
- Reduce batch: `--batch 8`
- Reduce image size: `--imgsz 416`

**Poor Detection:**
- Add more training data
- Check label quality
- Increase epochs: `--epochs 200`

**Slow Training:**
- Use GPU: `--device 0`
- Increase batch: `--batch 32`

---

**Ready to train?** Start with `python yolo/train_yolo_waste.py --create-yaml` 🚀

