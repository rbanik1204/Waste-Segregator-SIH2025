#!/usr/bin/env python3
"""
Dataset Preparation Script for Floating Waste Detection
Helps organize images and labels into YOLO format.

Usage:
    python prepare_dataset.py --source images/ --output datasets/waste --split 0.8
"""

import argparse
import os
import shutil
import random
from pathlib import Path
from collections import defaultdict

def split_dataset(source_dir, output_dir, train_ratio=0.8, seed=42):
    """
    Split dataset into train/val sets.
    
    Args:
        source_dir: Directory containing images and labels
        output_dir: Output directory for train/val split
        train_ratio: Ratio of training data (default: 0.8)
        seed: Random seed for reproducibility
    """
    random.seed(seed)
    
    source = Path(source_dir)
    output = Path(output_dir)
    
    # Create directory structure
    train_img_dir = output / 'train' / 'images'
    train_lbl_dir = output / 'train' / 'labels'
    val_img_dir = output / 'val' / 'images'
    val_lbl_dir = output / 'val' / 'labels'
    
    for d in [train_img_dir, train_lbl_dir, val_img_dir, val_lbl_dir]:
        d.mkdir(parents=True, exist_ok=True)
    
    # Find all images
    image_extensions = {'.jpg', '.jpeg', '.png', '.bmp'}
    images = []
    
    # Look for images in source directory
    for ext in image_extensions:
        images.extend(source.glob(f'*{ext}'))
        images.extend(source.glob(f'*{ext.upper()}'))
    
    if not images:
        print(f"❌ No images found in {source_dir}")
        return
    
    print(f"✓ Found {len(images)} images")
    
    # Shuffle and split
    random.shuffle(images)
    split_idx = int(len(images) * train_ratio)
    train_images = images[:split_idx]
    val_images = images[split_idx:]
    
    print(f"✓ Train: {len(train_images)} images")
    print(f"✓ Val: {len(val_images)} images")
    
    # Copy images and labels
    copied_train = 0
    copied_val = 0
    
    for img_path in train_images:
        # Copy image
        shutil.copy2(img_path, train_img_dir / img_path.name)
        
        # Copy corresponding label if exists
        label_path = source / (img_path.stem + '.txt')
        if label_path.exists():
            shutil.copy2(label_path, train_lbl_dir / label_path.name)
            copied_train += 1
    
    for img_path in val_images:
        # Copy image
        shutil.copy2(img_path, val_img_dir / img_path.name)
        
        # Copy corresponding label if exists
        label_path = source / (img_path.stem + '.txt')
        if label_path.exists():
            shutil.copy2(label_path, val_lbl_dir / label_path.name)
            copied_val += 1
    
    print(f"\n✓ Copied {copied_train} train labels")
    print(f"✓ Copied {copied_val} val labels")
    print(f"\n✓ Dataset prepared at: {output}")
    print(f"\nNext steps:")
    print(f"1. Annotate images using LabelImg (https://github.com/HumanSignal/labelImg)")
    print(f"2. Ensure labels are in YOLO format (class_id x_center y_center width height)")
    print(f"3. Update waste_dataset.yaml with correct class names")
    print(f"4. Run training: python train_yolo_waste.py --data waste_dataset.yaml --epochs 100")

def validate_dataset(dataset_dir):
    """Validate dataset structure and labels."""
    dataset = Path(dataset_dir)
    
    print("Validating dataset structure...")
    
    # Check directories
    required_dirs = [
        dataset / 'train' / 'images',
        dataset / 'train' / 'labels',
        dataset / 'val' / 'images',
        dataset / 'val' / 'labels'
    ]
    
    for d in required_dirs:
        if not d.exists():
            print(f"❌ Missing directory: {d}")
            return False
    
    # Count images and labels
    train_imgs = list((dataset / 'train' / 'images').glob('*'))
    train_lbls = list((dataset / 'train' / 'labels').glob('*.txt'))
    val_imgs = list((dataset / 'val' / 'images').glob('*'))
    val_lbls = list((dataset / 'val' / 'labels').glob('*.txt'))
    
    print(f"\nDataset statistics:")
    print(f"Train images: {len(train_imgs)}")
    print(f"Train labels: {len(train_lbls)}")
    print(f"Val images: {len(val_imgs)}")
    print(f"Val labels: {len(val_lbls)}")
    
    # Check for missing labels
    train_img_names = {img.stem for img in train_imgs}
    train_lbl_names = {lbl.stem for lbl in train_lbls}
    missing_train = train_img_names - train_lbl_names
    
    val_img_names = {img.stem for img in val_imgs}
    val_lbl_names = {lbl.stem for lbl in val_lbls}
    missing_val = val_img_names - val_lbl_names
    
    if missing_train:
        print(f"\n⚠ Warning: {len(missing_train)} train images without labels")
    if missing_val:
        print(f"⚠ Warning: {len(missing_val)} val images without labels")
    
    # Validate label format
    print("\nValidating label format...")
    errors = 0
    
    for lbl_path in list(train_lbls)[:10] + list(val_lbls)[:10]:  # Sample check
        try:
            with open(lbl_path, 'r') as f:
                for line_num, line in enumerate(f, 1):
                    parts = line.strip().split()
                    if len(parts) != 5:
                        print(f"❌ {lbl_path}:{line_num} - Invalid format (expected 5 values)")
                        errors += 1
                        continue
                    
                    class_id, x, y, w, h = map(float, parts)
                    if not (0 <= x <= 1 and 0 <= y <= 1 and 0 <= w <= 1 and 0 <= h <= 1):
                        print(f"❌ {lbl_path}:{line_num} - Values out of range [0,1]")
                        errors += 1
        except Exception as e:
            print(f"❌ Error reading {lbl_path}: {e}")
            errors += 1
    
    if errors == 0:
        print("✓ Label format validation passed")
    
    return True

def main():
    parser = argparse.ArgumentParser(
        description='Prepare dataset for YOLO training',
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    parser.add_argument('--source', type=str, required=True,
                       help='Source directory with images and labels')
    parser.add_argument('--output', type=str, default='datasets/waste',
                       help='Output directory for train/val split (default: datasets/waste)')
    parser.add_argument('--split', type=float, default=0.8,
                       help='Train/val split ratio (default: 0.8)')
    parser.add_argument('--validate', type=str, metavar='DATASET_DIR',
                       help='Validate existing dataset structure')
    parser.add_argument('--seed', type=int, default=42,
                       help='Random seed (default: 42)')
    
    args = parser.parse_args()
    
    if args.validate:
        validate_dataset(args.validate)
    else:
        split_dataset(args.source, args.output, args.split, args.seed)

if __name__ == '__main__':
    main()

