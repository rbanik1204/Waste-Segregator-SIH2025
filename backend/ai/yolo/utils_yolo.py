# Optional utility functions for YOLO
# ai/yolo/utils_yolo.py
# Mapping from object label -> (category, subtype)
# Updated for floating waste detection classes

WASTE_MAP = {
	# DRY WASTE - Plastic
	"plastic": ("dry", "plastic"),
	"plastic_bottle": ("dry", "plastic"),
	"plastic_bag": ("dry", "plastic"),
	"plastic_wrapper": ("dry", "plastic"),
	"plastic_cup": ("dry", "plastic"),
	"bottle": ("dry", "plastic"),
	"cup": ("dry", "plastic"),
	"wrapper": ("dry", "plastic"),
	"bag": ("dry", "plastic"),
	"packet": ("dry", "plastic"),
	"floating_plastic": ("dry", "plastic"),
	
	# DRY WASTE - Paper
	"paper": ("dry", "paper"),
	"cardboard": ("dry", "cardboard"),
	"newspaper": ("dry", "paper"),
	"book": ("dry", "paper"),
	"notebook": ("dry", "paper"),
	"carton": ("dry", "cardboard"),
	"box": ("dry", "cardboard"),
	
	# DRY WASTE - Metal
	"metal_can": ("dry", "metal"),
	"metal_scrap": ("dry", "metal"),
	"can": ("dry", "metal"),
	"tin": ("dry", "metal"),
	"metal": ("dry", "metal"),
	
	# DRY WASTE - Glass
	"glass_bottle": ("dry", "glass"),
	"glass_jar": ("dry", "glass"),
	"glass": ("dry", "glass"),
	"jar": ("dry", "glass"),
	
	# DRY WASTE - Textile
	"textile": ("dry", "textile"),
	"cloth": ("dry", "textile"),
	"fabric": ("dry", "textile"),
	
	# DRY WASTE - Other
	"electronic": ("dry", "e-waste"),
	"charger": ("dry", "e-waste"),
	"floating_wood": ("dry", "wood"),
	"floating_debris": ("dry", "debris"),

	# WET WASTE
	"food_waste": ("wet", "food"),
	"food": ("wet", "food"),
	"fruit": ("wet", "food"),
	"vegetable": ("wet", "food"),
	"banana": ("wet", "food"),
	"peel": ("wet", "food"),
	"organic_matter": ("wet", "organic"),
	"leaf": ("wet", "organic"),
	"plant_debris": ("wet", "organic"),
	"plant": ("wet", "organic"),
	"organic": ("wet", "organic"),
	"liquid": ("wet", "liquid"),

	# HAZARDOUS WASTE
	"battery": ("hazard", "battery"),
	"broken_glass": ("hazard", "broken_glass"),
	"broken": ("hazard", "broken_glass"),  # generic
	"sharp_object": ("hazard", "sharp"),
	"needle": ("hazard", "sharp"),
	"knife": ("hazard", "sharp"),
	"chemical_container": ("hazard", "chemical"),
	"medical_waste": ("hazard", "medical"),
	"syringe": ("hazard", "medical"),
	"mask": ("hazard", "medical"),
	"chemical": ("hazard", "chemical"),
	"paint": ("hazard", "chemical"),
	"acid": ("hazard", "chemical"),
	"oil_slick": ("hazard", "oil"),
	"hazardous_liquid": ("hazard", "chemical"),
}

def classify_label(label: str):
	"""
	Map a model label string to (category, subtype).
	Returns ("unknown", "unknown") if no mapping present.
	"""
	if not label:
		return ("unknown", "unknown")
	key = label.lower().replace(" ", "_")
	return WASTE_MAP.get(key, ("unknown", "unknown"))
