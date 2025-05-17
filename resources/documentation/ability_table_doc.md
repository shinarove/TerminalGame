### Documentation for the ability table

Reference: [resources/game_data/ability/ability_table.csv](../game_data/ability/ability_table.csv)

#### Description of the columns

- id:
  - Type: int
  - Description: Zero-base unique identifier for the ability. Must be the same as the (row index - 2) of the csv file.
  - Example: 0, 1, 2, 3, etc.
- name:
  - Type: str
  - Description: The key name of the ability. This is used to get the local string name of the ability.
  - Example: "ABILITY.BITE.NAME"
- c_target:
  - Type: char
  - Remarks: Strings are allowed, but only the first character is used. This is to improve readability.
  - Description: The character ability target.
  - Valid Values: 's' for self-target, 'e' for enemy target.
- r_target:
  - Type: char
  - Remarks: Strings are allowed, but only the first character is used. This is to improve readability.
  - Description: The resource ability target. (Which resource of the target is affected?)
  - Valid Values: 'h' for health, 's' for stamina, 'm' for mana
- effect_type:
  - Type: char
  - Remarks: Strings are allowed, but only the first character is used. This is to improve readability.
  - Description: Thy effect type of the ability.
  - Valid Values: 'd' for damage, 'h' for heal
- effect_scaler:
  - Type: char
  - Remarks: Strings are allowed, but only the first character is used. This is to improve readability.
  - Description: The effect scaler of the ability.
  - Valid Values: 's' for strength, 'i' for intelligence, 'a' for agility, 'c' for constitution, 'l' for luck
- accuracy_scaler:
  - Type: char
  - Remarks: Strings are allowed, but only the first character is used. This is to improve readability.
  - Description: The accuracy scaler of the ability.
  - Valid Values: 's' for strength, 'i' for intelligence, 'a' for agility, 'c' for constitution, 'l' for luck
- r_cost:
  - Type: char
  - Remarks: Strings are allowed, but only the first character is used. This is to improve readability.
  - Description: The resource cost of the ability. (Which resource is used to cast the ability?)
  - Valid Values: 'h' for health, 's' for stamina, 'm' for mana
- effect_dice:
  - Type: int
  - Description: The dice used to calculate the effect of the ability. (Number of sides)
  - Valid Values: 3, 4, 6, 8, 10, 12, 20
- accuracy_dice:
  - Type: int
  - Description: The dice used to calculate the accuracy of the ability. (Number of sides)
  - Valid Values: 3, 4, 6, 8, 10, 12, 20
- effect_rolls:
  - Type: int
  - Description: The number of rolls used to calculate the effect of the ability.
  - Example: 1, 2, 3, etc., when 0 the calculation is always 0.
- accuracy_rolls:
  - Type: int
  - Description: The number of rolls used to calculate the accuracy of the ability.
  - Example: 1, 2, 3, etc., when 0 the calculation is always 0.
- effect_scale_value:
  - Type: float
  - Description: The scale value which will be multiplied with the character attributes (based on the effect_scaler).
  This calculated value will be added to the rolled effect value.
  - Example: 0.5, 1.0, 1.5, etc.
- accuracy_scale_value:
  - Type: float
  - Description: The scale value which will be multiplied with the character attributes (based on the accuracy_scaler).
  This calculated value will be added to the rolled accuracy value.
  - Example: 0.5, 1.0, 1.5, etc.
- v_cost:
  - Type: int
  - Description: The value cost of the ability. (How much resource is used to cast the ability?)
  - Example: 0, 1, 2, etc.
