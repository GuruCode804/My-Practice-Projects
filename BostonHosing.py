import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import tensorflow as tf
from sklearn.model_selection import train_test_split
column_names = ['CRIM', 'ZN', 'INDUS', 'CHAS', 'NOX', 'RM', 'AGE', 'DIS', 'RAD', 'TAX', 'PTRATIO', 'B', 'LSTAT', 'MEDV']
housing=pd.read_csv("housing.csv",header=None, delimiter=r"\s+",names=column_names)
# print (housing)
# housing_one_hot=pd.get_dummies(housing,dtype=np.int32)
X=housing.drop("MEDV",axis=1)
y=housing["MEDV"]
X_train,X_test,y_train,y_test=train_test_split(X,y,test_size=0.2,random_state=42)
tf.random.set_seed(42)
model=tf.keras.Sequential([
    tf.keras.layers.Dense(200,activation="relu"),
tf.keras.layers.Dense(100,activation="relu"),
    tf.keras.layers.Dense(50,activation="relu"),
tf.keras.layers.Dense(1)
],name="housing_model")
model.compile(loss=tf.keras.losses.mae,
            optimizer=tf.keras.optimizers.Adam(learning_rate=0.01),
                 metrics=["mae"]
)
model.fit(X_train,y_train,epochs=150,verbose=1)
model.evaluate(X_test,y_test)
print(model.predict(X_test)[5])
i = 5
pred = model.predict(X_test).flatten()[i]
actual = y_test.iloc[i]

print(f"Predicted: {pred:.2f}, Actual: {actual:.2f}")
# Make predictions
y_pred = model.predict(X_test).flatten()

# Plot actual vs predicted
plt.figure(figsize=(10,6))
plt.scatter(y_test, y_pred, color='blue', alpha=0.6)
plt.plot([y_test.min(), y_test.max()], [y_test.min(), y_test.max()], color='red', linestyle='--')
plt.xlabel("Actual MEDV (House Price)")
plt.ylabel("Predicted MEDV")
plt.title("Actual vs Predicted House Prices")
plt.grid(True)
plt.show()