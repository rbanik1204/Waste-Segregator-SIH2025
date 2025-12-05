# ai/yolo/inference_yolov8.py
# Enhanced inference script for floating waste detection
import argparse, sys, os, json
from pathlib import Path
from ultralytics import YOLO

parser = argparse.ArgumentParser(description='YOLOv8 inference for floating waste detection')
parser.add_argument("--image", required=True, help="Path to image file")
parser.add_argument("--model", default=None, help="Model path (auto-detected if not specified)")
parser.add_argument("--conf", type=float, default=0.25, help="Confidence threshold")
parser.add_argument("--iou", type=float, default=0.45, help="IoU threshold for NMS")
parser.add_argument("--all", action="store_true", help="Return all detections, not just best")
args = parser.parse_args()

# Auto-detect model path (prioritize trained model)
ROOT = Path(__file__).resolve().parent
MODEL_CANDIDATES = [
    args.model,  # User-specified
    str(ROOT / "yolov8n.pt"),  # Trained model in yolo directory
    str(ROOT.parent.parent / "yolov8n.pt"),  # Root directory
    "./backend/ai/yolo/yolov8n.pt",  # Relative path
    "/mnt/data/yolov8n.pt"  # Alternative location
]

model_path = None
for p in MODEL_CANDIDATES:
    if p and os.path.exists(p):
        model_path = p
        print(f"✓ Using model: {p}", file=sys.stderr)
        break

if model_path is None:
    # Fallback: use pretrained model name (will download if needed)
    model_path = "yolov8n.pt"
    print(f"⚠ Model not found, using pretrained: {model_path}", file=sys.stderr)

try:
    model = YOLO(model_path)
except Exception:
    print("-")
    sys.exit(0)

# Run inference with optimized settings for floating waste
results = None
try:
    results = model.predict(
        args.image,
        conf=args.conf,
        iou=args.iou,
        verbose=False,
        imgsz=640,  # Match training size
        augment=False  # Disable augmentation for inference
    )
except Exception as e:
    print(f"Error during inference: {e}", file=sys.stderr)
    print("-")
    sys.exit(0)

if not results or len(results) == 0:
    print("-")
    sys.exit(0)

res = results[0]
if len(res.boxes) == 0:
    print("-")
    sys.exit(0)

# Get all detections
boxes = res.boxes
detections = []

try:
    for i, box in enumerate(boxes):
        try:
            cls = int(box.cls[0].item())
        except Exception:
            cls = int(box.cls[0])
        
        try:
            conf = float(box.conf[0].item())
        except Exception:
            conf = float(box.conf[0])
        
        # Get label name
        if hasattr(res, "names") and res.names:
            label = res.names.get(cls, f"class_{cls}")
        else:
            label = f"class_{cls}"
        
        # Get bounding box coordinates
        try:
            xyxy = box.xyxy[0].tolist() if hasattr(box.xyxy[0], 'tolist') else box.xyxy[0]
        except Exception:
            xyxy = [0, 0, 0, 0]
        
        detections.append({
            "label": label,
            "confidence": conf,
            "class_id": cls,
            "bbox": xyxy
        })
except Exception as e:
    print(f"Error processing detections: {e}", file=sys.stderr)
    print("-")
    sys.exit(0)

if not detections:
    print("-")
    sys.exit(0)

# Return all detections or just the best one
if args.all:
    # Return JSON with all detections
    output = {
        "detections": detections,
        "count": len(detections)
    }
    print(json.dumps(output))
else:
    # Return best detection (highest confidence)
    best = max(detections, key=lambda x: x["confidence"])
    label = best["label"]
    conf = best["confidence"]
    print(f"{label}:{conf:.2f}")

sys.exit(0)
