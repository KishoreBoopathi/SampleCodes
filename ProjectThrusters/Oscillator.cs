using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Oscillator : MonoBehaviour
{
    Vector3 startingPosition;
    [SerializeField] Vector3 movementVector;
    [SerializeField] [Range(0,1)] float movementFactor;
    [SerializeField] float period = 2f;
    // Start is called before the first frame update
    void Start()
    {
         startingPosition = transform.position;

    }

    // Update is called once per frame
    void Update()
    {
        if(period <= Mathf.Epsilon) { return; }
        float cycles = Time.time / period; //Keeps growing over time

        const float tau = Mathf.PI * 2; //constant value of 6.283 (Angle in radian of one complete cycle)
        float rawSinWave = Mathf.Sin(cycles * tau); //returns value between -1 and 1

        movementFactor = (rawSinWave + 1f) / 2f; //recalculated to range between 0 and 1

        Vector3 offset = movementVector * movementFactor;
        transform.position = startingPosition + offset;
    }
}
