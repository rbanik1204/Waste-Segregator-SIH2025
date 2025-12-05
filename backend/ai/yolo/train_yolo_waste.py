#!/usr/bin/env python3
"""
YOLOv8n Training Script for Floating Waste Detection
Optimized for detecting waste objects floating on water surfaces.

Usage:
    python train_yolo_waste.py --data datasets/waste_dataset.yaml --epochs 100 --imgsz 640
"""

import argparse
import os
import sys
from pathlib import Path
from ultralytics import YOLO
import yaml

# Default paths
ROOT = Path(__file__).resolve().parent
MODEL_DIR = ROOT
DATA_DIR = ROOT.parent.parent / "data" / "datasets"

def create_dataset_yaml(output_path, train_dir, val_dir, classes):
    """Create YOLO dataset YAML configuration file."""
    base_path = Path(train_dir).parent.absolute()
    train_rel = Path(train_dir).relative_to(base_path) / 'images'
    val_rel = Path(val_dir).relative_to(base_path) / 'images'
    
    yaml_content = {
        'path': str(base_path),
        'train': str(train_rel),
        'val': str(val_rel),
        'nc': len(classes),
        'names': classes
    }
    
    with open(output_path, 'w') as f:
        yaml.dump(yaml_content, f, default_flow_style=False, sort_keys=False)
    
    print(f"✓ Created dataset YAML: {output_path}")
    return output_path

def train_model(
    data_yaml,
    epochs=100,
    imgsz=640,
    batch=16,
    model_name='yolov8n.pt',
    device='cpu',
    patience=50,
    save_period=10
):
    """
    Train YOLOv8n model for waste detection.
    
    Args:
        data_yaml: Path to dataset YAML file
        epochs: Number of training epochs
        imgsz: Image size (640 recommended for floating objects)
        batch: Batch size
        model_name: Pretrained model name or path
        device: 'cpu', 'cuda', or '0' for GPU
        patience: Early stopping patience
        save_period: Save checkpoint every N epochs
    """
    
    # Initialize model
    model_path = MODEL_DIR / model_name
    if not model_path.exists():
        print(f"⚠ Model {model_path} not found, using pretrained {model_name}")
        model = YOLO(model_name)  # Will download if needed
    else:
        print(f"✓ Loading model from {model_path}")
        model = YOLO(str(model_path))
    
    # Training configuration optimized for floating waste detection
    results = model.train(
        data=str(data_yaml),
        epochs=epochs,
        imgsz=imgsz,
        batch=batch,
        device=device,
        patience=patience,
        save_period=save_period,
        
        # Data augmentation for water/floating objects
        hsv_h=0.015,      # Hue augmentation (water color variations)
        hsv_s=0.7,        # Saturation augmentation (water reflections)
        hsv_v=0.4,        # Value augmentation (lighting on water)
        degrees=10,       # Rotation (±10° for floating orientation)
        translate=0.1,    # Translation
        scale=0.5,        # Scale augmentation (distance variations)
        shear=2,          # Shear transformation
        perspective=0.0001, # Perspective (water surface angle)
        flipud=0.0,       # No vertical flip (waste floats on top)
        fliplr=0.5,       # Horizontal flip OK
        mosaic=1.0,       # Mosaic augmentation
        mixup=0.1,        # Mixup augmentation (light for waste)
        copy_paste=0.1,   # Copy-paste augmentation
        
        # Training optimizations
        optimizer='AdamW',  # AdamW optimizer
        lr0=0.001,         # Initial learning rate
        lrf=0.01,          # Final learning rate factor
        momentum=0.937,    # SGD momentum
        weight_decay=0.0005, # Weight decay
        warmup_epochs=3,   # Warmup epochs
        warmup_momentum=0.8,
        warmup_bias_lr=0.1,
        
        # Loss function weights (important for multi-class waste)
        box=7.5,           # Box loss gain
        cls=0.5,           # Class loss gain
        dfl=1.5,           # DFL loss gain
        
        # Validation
        val=True,          # Validate during training
        plots=True,        # Generate plots
        save=True,         # Save checkpoints
        save_json=False,   # Save JSON results
        
        # Model settings
        pretrained=True,   # Use pretrained weights
        verbose=True,      # Verbose output
        seed=42,           # Random seed
        deterministic=True,
        
        # Project and name
        project=str(MODEL_DIR / 'runs'),
        name='waste_detection',
        exist_ok=True,
        
        # Additional settings for floating objects
        close_mosaic=10,   # Disable mosaic in last 10 epochs
        resume=False,     # Resume from last checkpoint
        amp=True,          # Automatic Mixed Precision (faster training)
    )
    
    print("\n" + "="*60)
    print("✓ Training completed!")
    print("="*60)
    
    # Print best model path
    best_model = Path(results.save_dir) / 'weights' / 'best.pt'
    if best_model.exists():
        print(f"\n✓ Best model saved at: {best_model}")
        
        # Copy best model to main directory
        import shutil
        final_model = MODEL_DIR / 'yolov8n.pt'
        shutil.copy2(best_model, final_model)
        print(f"✓ Copied best model to: {final_model}")
    
    return results

def main():
    parser = argparse.ArgumentParser(
        description='Train YOLOv8n for floating waste detection',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Example usage:
  # Basic training
  python train_yolo_waste.py --data datasets/waste_dataset.yaml --epochs 100
  
  # With custom paths
  python train_yolo_waste.py --data datasets/waste_dataset.yaml --epochs 150 --imgsz 640 --batch 32
  
  # GPU training
  python train_yolo_waste.py --data datasets/waste_dataset.yaml --device 0 --epochs 100
        """
    )
    
    parser.add_argument('--data', type=str, required=True,
                       help='Path to dataset YAML file')
    parser.add_argument('--epochs', type=int, default=100,
                       help='Number of training epochs (default: 100)')
    parser.add_argument('--imgsz', type=int, default=640,
                       help='Image size (default: 640)')
    parser.add_argument('--batch', type=int, default=16,
                       help='Batch size (default: 16)')
    parser.add_argument('--model', type=str, default='yolov8n.pt',
                       help='Pretrained model name or path (default: yolov8n.pt)')
    parser.add_argument('--device', type=str, default='cpu',
                       help='Device: cpu, cuda, or 0,1,2,3 for GPU (default: cpu)')
    parser.add_argument('--patience', type=int, default=50,
                       help='Early stopping patience (default: 50)')
    parser.add_argument('--save-period', type=int, default=10,
                       help='Save checkpoint every N epochs (default: 10)')
    parser.add_argument('--create-yaml', action='store_true',
                       help='Create dataset YAML template and exit')
    
    args = parser.parse_args()
    
    # Create YAML template if requested
    if args.create_yaml:
        yaml_path = ROOT / 'waste_dataset.yaml'
        train_dir = DATA_DIR / 'waste' / 'train'
        val_dir = DATA_DIR / 'waste' / 'val'
        
        # Comprehensive waste classes for floating detection
        classes = [
            # Dry waste
            'plastic_bottle', 'plastic_bag', 'plastic_wrapper', 'plastic_cup',
            'paper', 'cardboard', 'newspaper',
            'metal_can', 'metal_scrap',
            'glass_bottle', 'glass_jar',
            'textile', 'cloth',
            
            # Wet waste
            'food_waste', 'fruit', 'vegetable', 'organic_matter',
            'leaf', 'plant_debris',
            
            # Hazardous waste
            'battery', 'broken_glass', 'sharp_object',
            'chemical_container', 'medical_waste',
            'oil_slick', 'hazardous_liquid',
            
            # Floating-specific
            'floating_plastic', 'floating_wood', 'floating_debris'
        ]
        
        create_dataset_yaml(yaml_path, train_dir, val_dir, classes)
        print(f"\n✓ Dataset YAML template created at: {yaml_path}")
        print(f"\nNext steps:")
        print(f"1. Organize your images and labels:")
        print(f"   {train_dir}/images/")
        print(f"   {train_dir}/labels/")
        print(f"   {val_dir}/images/")
        print(f"   {val_dir}/labels/")
        print(f"2. Use LabelImg or similar tool to annotate images")
        print(f"3. Run training: python train_yolo_waste.py --data {yaml_path} --epochs 100")
        return
    
    # Validate dataset YAML exists
    data_yaml = Path(args.data)
    if not data_yaml.exists():
        print(f"❌ Error: Dataset YAML not found: {data_yaml}")
        print(f"\nCreate it first:")
        print(f"  python train_yolo_waste.py --create-yaml")
        sys.exit(1)
    
    # Start training
    print("="*60)
    print("🚢 YOLOv8n Training for Floating Waste Detection")
    print("="*60)
    print(f"Dataset: {data_yaml}")
    print(f"Epochs: {args.epochs}")
    print(f"Image size: {args.imgsz}")
    print(f"Batch size: {args.batch}")
    print(f"Device: {args.device}")
    print("="*60 + "\n")
    
    try:
        results = train_model(
            data_yaml=args.data,
            epochs=args.epochs,
            imgsz=args.imgsz,
            batch=args.batch,
            model_name=args.model,
            device=args.device,
            patience=args.patience,
            save_period=args.save_period
        )
        
        print("\n✅ Training completed successfully!")
        print(f"📊 Results saved in: {results.save_dir}")
        print(f"🎯 Best model: {results.save_dir}/weights/best.pt")
        
    except KeyboardInterrupt:
        print("\n⚠ Training interrupted by user")
        sys.exit(1)
    except Exception as e:
        print(f"\n❌ Training failed: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == '__main__':
    main()

