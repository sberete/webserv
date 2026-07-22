#!/usr/bin/env python3
import random

verdicts = [
    ("Elle likait les stories de son ex.", "94%"),
    ("Tu repondais avec un seul mot.", "88%"),
    ("Le zodiaque, en fait.", "12%"),
    ("Il/elle n'etait juste pas pret(e).", "3%"),
    ("Tu ne mettais jamais la table.", "67%"),
]
verdict, confidence = random.choice(verdicts)

print("Content-Type: text/html")
print()
print(f"""<!DOCTYPE html>
<html>
<body>
  <h1>Verdict</h1>
  <p><strong>{verdict}</strong></p>
  <p>Confiance : {confidence}</p>
  <a href="/">Retour</a>
</body>
</html>""")
