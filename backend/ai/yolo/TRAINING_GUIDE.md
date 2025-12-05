# YOLOv8n Training Guide for Floating Waste Detection

This guide will help you train a YOLOv8n model specifically optimized for detecting waste objects floating on water surfaces.

## 📋 Prerequisites

1. **Python 3.8+** with pip
2. **CUDA-capable GPU** (recommended, but CPU works too)
3. **Dataset of waste images** (floating on water preferred)

## 🚀 Quick Start

### Step 1: Install Dependencies

```bash
cd backend/ai
pip install -r docker/requirements.txt
```

### Step 2: Prepare Your Dataset

#### Option A: Use Existing Images
If you have images already:

```bash
python yolo/prepare_dataset.py --source /path/to/your/images --output data/datasets/waste --split 0.8
```

#### Option B: Create Dataset Structure Manually
```
data/datasets/waste/
├── train/
│   ├── images/
│   │   ├── img001.jpg
│   │   ├── img002.jpg
│   │   └── ...
│   └── labels/
│       ├── img001.txt
│       ├── img002.txt
│       └── ...
└── val/
    ├── images/
    │   ├── img101.jpg
    │   └── ...
    └── labels/
        ├── img101.txt
        └── ...
```

### Step 3: Annotate Images

Use **LabelImg** (https://github.com/HumanSignal/labelImg) to annotate your images:

1. Install LabelImg:
   ```bash
   pip install labelImg
   labelImg
   ```

2. Open LabelImg and:
   - Set format to **YOLO** (not PascalVOC)
   - Open your `train/images` directory
   - Annotate each image with bounding boxes
   - Save labels in `train/labels` directory

3. Repeat for validation images

#### Label Format (YOLO)
Each `.txt` file should contain one line per object:
```
class_id x_center y_center width height
```

All values are normalized (0-1):
- `class_id`: Integer (0, 1, 2, ...)
- `x_center`, `y_center`: Center coordinates
- `width`, `height`: Bounding box dimensions

#### Class Mapping
Update `waste_dataset.yaml` with your class names. Example classes:
- **Dry waste**: plastic_bottle, plastic_bag, paper, metal_can, glass_bottle, etc.
- **Wet waste**: food_waste, fruit, vegetable, organic_matter, leaf, etc.
- **Hazardous**: battery, broken_glass, sharp_object, chemical_container, etc.

### Step 4: Create Dataset YAML

```bash
python yolo/train_yolo_waste.py --create-yaml
```

This creates `waste_dataset.yaml`. Edit it to match your class names.

### Step 5: Train the Model

#### Basic Training (CPU):
```bash
python yolo/train_yolo_waste.py --data yolo/waste_dataset.yaml --epochs 100
```

#### GPU Training:
```bash
python yolo/train_yolo_waste.py --data yolo/waste_dataset.yaml --epochs 100 --device 0 --batch 32
```

#### Advanced Training:
```bash
python yolo/train_yolo_waste.py \
    --data yolo/waste_dataset.yaml \
    --epochs 150 \
    --imgsz 640 \
    --batch 32 \
    --device 0 \
    --patience 50
```

### Step 6: Monitor Training

Training progress is saved in:
```
backend/ai/yolo/runs/waste_detection/
├── weights/
│   ├── best.pt      # Best model (use this!)
│   └── last.pt      # Latest checkpoint
├── results.png      # Training curves
├── confusion_matrix.png
└── ...
```

### Step 7: Use Trained Model

The best model is automatically copied to:
```
backend/ai/yolo/yolov8n.pt
```

The inference API (`backend/ai/api/main.py`) will use this model automatically.

## 🎯 Training Tips for Floating Waste

### 1. **Dataset Quality**
- Include images with water reflections, waves, and different lighting
- Capture waste at various distances and angles
- Include partially submerged objects
- Add images with multiple objects

### 2. **Data Augmentation** (Already Configured)
The training script includes:
- **HSV augmentation**: Handles water color variations
- **Rotation**: Accounts for floating orientation
- **Scale**: Distance variations
- **Mosaic**: Multi-object scenarios
- **No vertical flip**: Waste floats on top (flipud=0.0)

### 3. **Class Balance**
Ensure balanced classes:
- Dry waste: ~40%
- Wet waste: ~30%
- Hazardous: ~20%
- Floating-specific: ~10%

### 4. **Training Parameters**
- **Image size**: 640px (good balance of speed/accuracy)
- **Batch size**: 16 (CPU) or 32+ (GPU)
- **Epochs**: 100-150 (monitor for overfitting)
- **Learning rate**: Auto-adjusted (starts at 0.001)

### 5. **Early Stopping**
Training stops automatically if no improvement for 50 epochs (configurable).

## 📊 Validation

Validate your dataset before training:
```bash
python yolo/prepare_dataset.py --validate data/datasets/waste
```

## 🔧 Troubleshooting

### Out of Memory
- Reduce batch size: `--batch 8`
- Reduce image size: `--imgsz 416`

### Poor Detection
- Add more training data
- Check label quality
- Increase epochs: `--epochs 200`
- Fine-tune learning rate

### Slow Training
- Use GPU: `--device 0`
- Increase batch size: `--batch 32`
- Use smaller model: `yolov8n.pt` (already using)

## 📈 Expected Results

After training, you should see:
- **mAP50**: >0.70 (good)
- **mAP50-95**: >0.50 (good)
- **Precision**: >0.75
- **Recall**: >0.70

## 🎓 Next Steps

1. Test inference: `python yolo/inference_yolov8.py --image test.jpg`
2. Integrate with API: The FastAPI service uses the trained model
3. Monitor in production: Check dashboard for detection accuracy
4. Retrain periodically: Add new data and retrain for better accuracy

## 📚 Resources

- [Ultralytics YOLOv8 Docs](https://docs.ultralytics.com/)
- [LabelImg GitHub](https://github.com/HumanSignal/labelImg)
- [YOLO Format Guide](https://roboflow.com/formats/yolo-annotation)

---

**Happy Training! 🚢♻️**

