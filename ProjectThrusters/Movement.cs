using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Movement : MonoBehaviour
{
    Rigidbody rocketRigidbody;
    AudioSource rocketAudioSource;

    [SerializeField] float thrustSpeed = 1000.0f;
    [SerializeField] float rotateAngle = 100.0f;
    [SerializeField] AudioClip mainEngine;
    [SerializeField] ParticleSystem thrustFire;
    [SerializeField] ParticleSystem thrustFireParticle;
    [SerializeField] ParticleSystem thrustSmoke;

    // Start is called before the first frame update
    void Start()
    {
        rocketRigidbody = GetComponent<Rigidbody>();
        rocketAudioSource = GetComponent<AudioSource>();

    }
    // Update is called once per frame
    void Update()
    {
        ProcessThrust();
        ProcessRotation();
    }
    void ProcessThrust()
    {
        if(Input.GetKey(KeyCode.Space))
        {
            InitiateThrusting();
        }
        else
        {
            StopThrusting();
        }
    }
    void InitiateThrusting()
    {
        rocketRigidbody.AddRelativeForce(Vector3.up * thrustSpeed * Time.deltaTime);
        if (!rocketAudioSource.isPlaying)
            rocketAudioSource.PlayOneShot(mainEngine);
        if (!thrustFire.isPlaying)
            PlayThrustParticleEffect();
    }
    void StopThrusting()
    {
        rocketAudioSource.Stop();
        StopThrustParticleEffect();
    }

    void ProcessRotation()
    {
        if(Input.GetKey(KeyCode.A))
        { 
            ApplyRotation(rotateAngle);
        }
        else if(Input.GetKey(KeyCode.D))
        {
            ApplyRotation(-rotateAngle);
        }
    }
    void ApplyRotation(float rotationThisFrame)
    {
        rocketRigidbody.freezeRotation = true;
        transform.Rotate(Vector3.forward * rotationThisFrame * Time.deltaTime, Space.World);
        rocketRigidbody.freezeRotation = false;
    }

    void PlayThrustParticleEffect()
    {
        thrustFire.Play();
        thrustFireParticle.Play();
        thrustSmoke.Play();
    }

    void StopThrustParticleEffect()
    {
        thrustFire.Stop();
        thrustFireParticle.Stop();
        thrustSmoke.Stop();
    }
}
